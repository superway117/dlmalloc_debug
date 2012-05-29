/*  -------------------------------------------------------------------------
    Copyright (C) 2005 - 2010 INFINEON TECHNOLOGIES AG (IFAG)
    All rights reserved.
    -------------------------------------------------------------------------
    This document contains proprietary information belonging to INFINEON.
    Passing on and copying of this document, use and communication of its
    contents is not permitted without prior written authorization.
    -------------------------------------------------------------------------
    Revision Information:
       $File name:  /red_arrow/Source/Engine/Memory/c166_malloc.c $
       $CC-Version: .../oint_redarrow_engine_xmm2138/5 $
       $Date:       2010-09-29    12:52:13 UTC $
    ------------------------------------------------------------------------- */

/* =============================================================================
** Copyright (C) 2001 COMNEON GmbH & Co. OHG. All rights reserved.
** =============================================================================
**
** =============================================================================
**
** This document contains proprietary information belonging to COMNEON.
** Passing on and copying of this document, use and communication of its
** contents is not permitted without prior written authorisation.
**
** =============================================================================
*/
#include "portab_types.h"
#include <stdarg.h>     /* va_list */
#ifndef WIN32
//#include <trap.h>
#else
#include "process.h" // exit(0)
#endif
#include "dl_allocator.h"
#include "tg_allocator.h"

#include <uta_fs.h>
//#include <uta_common_types.h>

typedef struct
{
    CHAR       *heap_name;
    UINT32      heap_size;
    UINT8      *heap;
    UINT32      lmtLow;
    UINT32      lmtHigh;
}TG_HEAP;

extern void    hw_ClockGetTimeDate ( MMI_TIME_DATE   *uhParams );


#if  MMI_GET_HEAP_PEAK_VALUE
static UINT32 s_tg_heap_peak;
extern  UINT32 	tg_get_total_used_heap_size( void );
#endif


/* Define the heap pools */

#ifdef WIN32
#pragma pack(push)
#pragma pack(1)
#define TG_HEAP_POOL_DEFINE( id, name, size, limitLow, limitHigh ) \
static UINT8 s_##id##_heap[size];
#include "tg_allocator_config.h"
#pragma pack(pop)
#else
#define TG_HEAP_POOL_DEFINE( id, name, size, limitLow, limitHigh ) \
__align(16) static UINT8 s_##id##_heap[size];
#include "tg_allocator_config.h"
#endif
#undef TG_HEAP_POOL_DEFINE



/* MMI heap table */
const TG_HEAP s_tg_heap[HEAP_NUM]=
{
#define TG_HEAP_POOL_DEFINE( id, name, size, limitLow, limitHigh ) \
    { name, size, s_##id##_heap, limitLow, limitHigh },
#include "tg_allocator_config.h"
#undef TG_HEAP_POOL_DEFINE
};

static void* s_tg_heap_ms[HEAP_NUM];

#ifdef MEM_ALLOCATOR_DEBUG
void red_arrow_exit()
{
    tg_heap_traverse(NO_PRINT);
}
#endif

void 	tg_heap_init( void )
{
    UINT32 i =0 ;
    static BOOL s_is_init = FALSE;
    
#ifdef MEM_ALLOCATOR_DEBUG
    extern int atexit(void (*f)());
    atexit(red_arrow_exit);
#endif
	if(s_is_init == TRUE)
		return;
    for (;i<HEAP_NUM;i++)
    {
        s_tg_heap_ms[i] = dl_create_mspace_with_base(s_tg_heap[i].heap,s_tg_heap[i].heap_size,TRUE);
    }
    s_is_init = TRUE;
}


#if  MMI_GET_HEAP_PEAK_VALUE
void tg_heap_set_peak()
{
    /* this is not thread safe, but used for debugging/config purpose only! */
    UINT32 tatal_peak = tg_get_total_used_heap_size();
    if (tatal_peak> s_tg_heap_peak)
        s_tg_heap_peak = tatal_peak;

}
#endif

void *	tg_malloc_in_heap( UINT32 size ,TG_HEAP_ID heap_id, int line, char * file_name)
{
    void* buf = NULL;

    if ( size == 0 )
        return NULL;

    buf = dl_mspace_malloc(s_tg_heap_ms[heap_id],size,line,file_name);

#if  MMI_GET_HEAP_PEAK_VALUE
    tg_heap_set_peak();
#endif

    return buf;
}


void *	tg_malloc_in_heap_exit_on_null( UINT32 size ,TG_HEAP_ID heap_id, int line, char * file_name)
{
    void* buf = tg_malloc_in_heap(size,heap_id,line,file_name);

    if (buf == NULL)
        tg_exit(0);

    return buf;
}



//default malloc means dynamic malloc
#if MMI_GET_ALLOCATOR_PERFORMANCE
extern  unsigned int RTT_stm_get_time_1us_resolution(void);
#endif
void *	tg_malloc( UINT32 size, int line, char * file_name)
{
    void* buf = NULL;
    UINT8 i;
#if MMI_GET_ALLOCATOR_PERFORMANCE
    UINT32 t1=RTT_stm_get_time_1us_resolution();
    UINT32 t2 = 0;
#endif

    if (size == 0)
        return NULL;

    for ( i=0; i<HEAP_NUM; i++ )
    {
        if ( size < s_tg_heap[i].lmtLow )
        {
            buf = dl_mspace_malloc(s_tg_heap_ms[i],size,line,file_name);
            if ( buf != NULL ) break;
        }
    }

    /* if can not find a place-holder with Low Limit,
     * try to find one with High Limit
     */
    if ( buf == NULL )
        for ( i=0; i<HEAP_NUM; i++ )
        {
            if ( size < s_tg_heap[i].lmtHigh )
            {
                buf = dl_mspace_malloc(s_tg_heap_ms[i],size,line,file_name);
                if ( buf != NULL ) break;
            }
        }

#if MMI_GET_ALLOCATOR_PERFORMANCE
    t2 = RTT_stm_get_time_1us_resolution();
    mmi_printf("tg_malloc use %dus",t2-t1);
#endif

#if  MMI_GET_HEAP_PEAK_VALUE
    tg_heap_set_peak();
#endif

    return buf;
}

void *	tg_malloc_exit_on_null( UINT32 size, int line, char * file_name)
{
    void* buf = NULL;
    if (size == 0)
        return NULL;
    buf = tg_malloc(size,line,file_name);
    if (buf == NULL)
        tg_exit(0);
    return buf;
}
void *	tg_calloc( UINT32 count ,UINT32 size, int line, char * file_name)
{
    UINT8* buf = NULL;
    UINT32 mem_size = count* size;
    buf =tg_malloc(mem_size,line,file_name);
    if (buf != NULL)
        memset(buf,0,mem_size);
    return buf;

}
void 	tg_free( void *block , int line, char * file_name)
{
#if MMI_GET_ALLOCATOR_PERFORMANCE
    UINT32 t1=RTT_stm_get_time_1us_resolution();
    UINT32 t2 = 0;
#endif
    if (block != NULL)
    {
        UINT32 i=0;
        for (;i<HEAP_NUM;i++)
        {
            if (dl_is_valid_address(s_tg_heap_ms[i],block))
            {
                dl_mspace_free(s_tg_heap_ms[i],block,line, file_name);
                break;
            }
        }
    }
#if MMI_GET_ALLOCATOR_PERFORMANCE
    t2 = RTT_stm_get_time_1us_resolution();
    mmi_printf("tg_free use %dus",t2-t1);
#endif
}
void *	tg_realloc( void *ptr, UINT32 size , int line, char * file_name)
{
    UINT8* buf = NULL;
    if (size==0)
        return NULL;
    buf=tg_malloc_exit_on_null(size,line,file_name);
    if (buf == NULL)
        return NULL;
    if (ptr!=NULL)
    {
        memcpy(buf,ptr,size);
        tg_free(ptr,line,file_name);
    }
    ptr =buf;
    return ptr;
}
INT32 tg_get_heap_current_high_water_by_heapid(TG_HEAP_ID heap_id)
{
    return  dl_get_current_high_water(s_tg_heap_ms[heap_id]);
}
INT32 tg_get_heap_high_water_by_heapid(UINT32 size,TG_HEAP_ID heap_id)
{
    return dl_get_high_water(s_tg_heap_ms[heap_id],size);
}



INT32 tg_get_heap_high_water(UINT32 size)
{
    INT32 used_size = 0;
    INT32 total_size = 0;
    INT32 i, level = 0;

    for ( i=0; i<HEAP_NUM; i++ )
    {
        total_size += dl_get_total_size(s_tg_heap_ms[i]);
        used_size += dl_get_used_size(s_tg_heap_ms[i]);
    }
    used_size+=size;

    level = (used_size)*100/total_size;

    return level;
}





INT32 	tg_get_heap_id( void *block )
{
    if (block != NULL)
    {
        UINT32 i=0;
        for (;i<HEAP_NUM;i++)
        {
            if (dl_is_valid_address(s_tg_heap_ms[i],block))
                return i;
        }
    }
    return -1;
}

UINT32 tg_get_usable_size(void* mem)
{
    return dl_get_usable_size(mem);
}

UINT32 tg_get_heap_used_size_by_heapid(TG_HEAP_ID heap_id)
{
    return dl_get_used_size(s_tg_heap_ms[heap_id]);
}

UINT32 tg_get_heap_free_size_by_heapid(TG_HEAP_ID heap_id)
{
    return dl_get_free_size(s_tg_heap_ms[heap_id]);
}

UINT32 	tg_get_total_used_heap_size( void )
{
    UINT32 size =0 ;
    TG_HEAP_ID heap_id = (TG_HEAP_ID)0;

    for (;heap_id<HEAP_NUM;heap_id++)
    {
        size+=tg_get_heap_used_size_by_heapid(heap_id);
    }
    return size;
}
UINT32 	tg_get_total_free_heap_size( void )
{
    UINT32 size =0 ;
    TG_HEAP_ID heap_id = (TG_HEAP_ID)0;

    for (;heap_id<HEAP_NUM;heap_id++)
    {
        size+=tg_get_heap_free_size_by_heapid(heap_id);
    }
    return size;
}
void tg_check_mspace_stats()
{
#if DL_DEBUG
    UINT32 i=0;

    for (;i<HEAP_NUM;i++)
    {
        dl_mspace_malloc_stats(s_tg_heap_ms[i]);

    }

#endif

}

//#ifdef MEM_ALLOCATOR_DEBUG
static UtaIoHdl s_file_handler = 0;
#define MEMORY_FILE_PRINTF_MAX_SIZE 200
INT32   allocator_fileprintf (UtaIoHdl hdl,const char *fmt, va_list args)
{
    INT32 length = 0;
    UINT8   temp_buffer[MEMORY_FILE_PRINTF_MAX_SIZE+1];//=MMI_MALLOC(VFS_FILE_PRINTF_MAX_SIZE);

#ifndef WIN32
    length = vsnprintf (temp_buffer, MEMORY_FILE_PRINTF_MAX_SIZE, fmt, args);
#else
    length = _vsnprintf (temp_buffer, MEMORY_FILE_PRINTF_MAX_SIZE, fmt, args);
#endif
    if (hdl>0)
    {
        UtaFsWriteFile (hdl, temp_buffer, length);
    }
    // MMI_FREE(temp_buffer);

    return length;
}
void allocator_printf(INT32 print_type, const char *fmt, ...)
{
    va_list a;
    va_start(a, fmt);
    if (print_type == NO_PRINT)
    {
        va_end(a);
        return;
    }
    else
    {

        if (print_type == FILE_PRINT)
            allocator_fileprintf(s_file_handler,fmt,a);
        else //lint -e437
            mmi_printf((char*)fmt,a);

    }
    va_end(a);
}
INT32 allocator_preaction(INT32 print_type)
{
    if (print_type == FILE_PRINT)
    {
        s_file_handler=0;
#ifndef WIN32
    //    if (vfs_GetFreeSpace("storage_card") > 100*1024)
#endif
        {
            CHAR filename[50];
            MMI_TIME_DATE  datetime;
            hw_ClockGetTimeDate( &datetime );
            sprintf(filename,"/storage_card/mmiheap_%d-%d-%d-%d-%d.txt",datetime.Date.Month,
                    datetime.Date.Day,datetime.Time.Hours,datetime.Time.Minutes,datetime.Time.Seconds);
            s_file_handler = UtaFsOpenFile(filename,UTA_FS_ACCESS_MODE_CREATE | UTA_FS_ACCESS_MODE_WRITE);
            if (s_file_handler>0)
                return FILE_PRINT;
        }
        print_type = NO_PRINT;
    }
    s_file_handler=0;
    return print_type;
}

void allocator_postaction(INT32 print_type)
{
    if (s_file_handler > 0 )
    {

        UtaFsCloseFile(s_file_handler);
        s_file_handler=0;
    }
}

BOOL tg_heap_traverse_impl(INT32 print_type,CHAR* error_log)
{
    BOOL result = TRUE;
    BOOL find_exception=FALSE;
#if DL_DEBUG
    UINT32 i=0;
    CHAR* log =NULL;;
    print_type = allocator_preaction(print_type);
    for (;i<HEAP_NUM;i++)
    {
        if (find_exception)
            log = NULL;
        else
            log = error_log;
        result=dl_heap_travese(s_tg_heap_ms[i],print_type,s_tg_heap[i].heap_name,log);
        allocator_printf(print_type, "\n");
        if (!result)
            find_exception=TRUE;
    }
#if  MMI_GET_HEAP_PEAK_VALUE
    allocator_printf(print_type, "------------------------Peak Value Summary----------------------------------------\n");
    allocator_printf(print_type, "Total peak value:\t%d\n",s_tg_heap_peak);
#endif
    allocator_postaction(print_type);
#endif
    return (!find_exception ? TRUE : FALSE); // for lint error 64
}
void tg_heap_traverse(INT32 print_type)
{
    tg_heap_traverse_impl(print_type,NULL);
}

void tg_dump_heap()
{
    UINT8          i;
    CHAR           filename[64];
    UtaIoHdl       file_handler = 0;
    MMI_TIME_DATE  datetime;

    hw_ClockGetTimeDate( &datetime );

    for ( i=0; i<HEAP_NUM; i++ )
    {
        memset( filename, 0, sizeof(filename) );
        sprintf(filename,"/storage_card/heap%d_dump_%d-%d-%d-%d-%d.txt",i,datetime.Date.Month,
                datetime.Date.Day,datetime.Time.Hours,datetime.Time.Minutes,datetime.Time.Seconds);
        file_handler = UtaFsOpenFile(filename,UTA_FS_ACCESS_MODE_CREATE | UTA_FS_ACCESS_MODE_WRITE);
        if ( file_handler >= 0 )
        {
            UtaFsWriteFile(file_handler, s_tg_heap[i].heap, s_tg_heap[i].heap_size);
            UtaFsCloseFile(file_handler);
        }
    }
}

void tg_exit(int status)
{
	#ifdef WIN32
	exit(status);
	#endif
	
}

void tg_heap_trap_exit()
{
#ifndef WIN32
    CHAR log[400];
    memset(log,0,400);
    if (!tg_heap_traverse_impl(NO_PRINT,log))
    {
#ifndef RED_ARROW_ON_UTA_V2	 // UTA_V2_TBD_XMM2138_NEW
//        TRAP_store_exception(TRAP_SW_EXCEPTION_TAG, strlen(log)+1, (void*)log);
#endif
    }
    
#endif
	tg_exit(0);

}

