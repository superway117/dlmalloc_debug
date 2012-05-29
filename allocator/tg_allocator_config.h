
#ifndef TG_HEAP_POOL_DEFINE
#define TG_HEAP_POOL_DEFINE( id, name, size, limitLow, limitHigh )
#endif

/**
 * Pool definition for MMI DL-Allocator
 *
 * Pool ID:           "XXXX_HEAP" is the actual heap ID defined in MMI_HEAP_ID
 * Pool Name:         Pool Name
 * Pool Size:         whole size of the pool in bytes
 * Low Limit:         max allowed alloc size in normal case. if no limit, set it same as Pool Size.
 * High Limit:        max allowed alloc size in emergency case. if no limit, set it same as Pool Size.
 *
 *                    Pool ID          Pool Name                Pool Size        Low Limit         High Limit
 */
TG_HEAP_POOL_DEFINE( DYNAMIC_SMALL,   "Dynamic Small Heap",    (128*1024),         256,                 256        )
#ifdef WIN32                                                                                            
TG_HEAP_POOL_DEFINE( DYNAMIC_LARGE,   "Dynamic Large Heap",    (1024*1024*6),      (400*1024),          (800*1024) )
TG_HEAP_POOL_DEFINE( DYNAMIC_HUGE,    "Dynamic Huge Heap",     (1024*3072*2),      (1024*3072*2),       (1024*3072*2) )
#else                                                                                                      
TG_HEAP_POOL_DEFINE( DYNAMIC_LARGE,   "Dynamic Large Heap",    (1024*2048),        (400*1024),          (800*1024) )
TG_HEAP_POOL_DEFINE( DYNAMIC_HUGE,    "Dynamic Huge Heap",     (1024*1638),        (1024*1638),         (1024*1638) )
#endif
TG_HEAP_POOL_DEFINE( STATIC,          "Static Heap",           (128*1024),         (128*1024),          (128*1024) )

