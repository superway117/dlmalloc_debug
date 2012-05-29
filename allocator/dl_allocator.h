/*  -------------------------------------------------------------------------
    Copyright (C) 2010 INFINEON TECHNOLOGIES AG (IFAG)
    All rights reserved.
    -------------------------------------------------------------------------
    This document contains proprietary information belonging to INFINEON.
    Passing on and copying of this document, use and communication of its
    contents is not permitted without prior written authorization.
    -------------------------------------------------------------------------
    Revision Information:
       $File name:  /red_arrow/Source/Engine/Memory/dl_allocator.h $
       $CC-Version: .../oint_redarrow_engine_xmm2138/4 $
       $Date:       2010-09-29    13:27:03 UTC $
    ------------------------------------------------------------------------- */

#ifndef __DL_ALLOCATOR_H__
#define __DL_ALLOCATOR_H__

#include <portab_types.h>
#include "dl_allocator_config.h"

extern void* dl_create_mspace_with_base(void* base, tg_size_t capacity, INT32 locked);

extern void* dl_mspace_malloc(void* msp, tg_size_t bytes,int line, char * file_name);

extern void dl_mspace_free(void* msp, void* mem,int line, char * file_name);

extern UINT32 dl_get_peak_size(void* msp);

extern INT32 dl_get_current_high_water(void*  msp);

extern INT32 dl_get_high_water(void* msp,UINT32 size);

extern UINT32 dl_get_usable_size(void* mem);

extern UINT32 dl_get_total_size(void* msp);

extern UINT32 dl_get_used_size(void* msp);

extern UINT32 dl_get_free_size(void* msp) ;

extern BOOL dl_is_valid_address(void* msp,void* addr) ;

extern void dl_mspace_malloc_stats(void* msp);

#if DL_DEBUG
extern BOOL dl_heap_travese(void* msp,INT32 print_type,const CHAR* heap_name,CHAR* error_log);
#endif

#endif
