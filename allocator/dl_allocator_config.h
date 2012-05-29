/*  -------------------------------------------------------------------------
    Copyright (C) 2010 INFINEON TECHNOLOGIES AG (IFAG)
    All rights reserved.
    -------------------------------------------------------------------------
    This document contains proprietary information belonging to INFINEON.
    Passing on and copying of this document, use and communication of its
    contents is not permitted without prior written authorization.
    -------------------------------------------------------------------------
    Revision Information:
       $File name:  /red_arrow/Source/Engine/Memory/dl_allocator_config.h $
       $CC-Version: .../oint_redarrow_engine_xmm2138/4 $
       $Date:       2010-09-29    13:57:05 UTC $
    ------------------------------------------------------------------------- */

#ifndef __DL_ALLOCATOR_CONFIG_H__
#define __DL_ALLOCATOR_CONFIG_H__

#include "portab_types.h"
#include "common.h"

typedef  UINT32 tg_size_t;

extern void tg_exit(int status);
//to get all heaps peak value
#define MMI_GET_HEAP_PEAK_VALUE			 1

//to get the performance
#define MMI_GET_ALLOCATOR_PERFORMANCE	 0

//debug mode
#define MEM_ALLOCATOR_DEBUG
#ifdef MEM_ALLOCATOR_DEBUG
#define DL_DEBUG 1
#define DEBUG 1
#else
#define DL_DEBUG 0
#define DEBUG 0
#endif

#define DL_DEEP_DEBUG 0

//!!!please do not change
#define ONLY_MSPACES  1
#define USE_LOCKS 1
#define FOOTERS   1
#define MALLOC_ALIGNMENT ((tg_size_t)16U)
#define MALLOC_FAILURE_ACTION
#define MORECORE_CANNOT_TRIM
#define HAVE_MMAP 0
#define malloc_getpagesize 0
#define MALLINFO_FIELD_TYPE UINT32
#define DEFAULT_GRANULARITY 0
#define ABORT  tg_exit(0)//abort()
#define PROCEED_ON_ERROR 0

//optional
#define INSECURE  1

#define CORRUPTION_ERROR_ACTION(m) ABORT
#define USAGE_ERROR_ACTION(m,p) ABORT

#endif
