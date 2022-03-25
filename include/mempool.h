#pragma once
#ifndef MEMPOOL_H
#define MEMPOOL_H

#include <stddef.h>

#ifdef __cplusplus
#define MEMPOOL_EXPORT extern "C"
#else
#define MEMPOOL_EXPORT
#endif

#ifdef _MSC_VER
#define MEMPOOL_API __cdecl
#else
#define MEMPOOL_API __attribute__((cdecl))
#endif

typedef struct _mempool *mempool;

MEMPOOL_EXPORT mempool MEMPOOL_API
mempool_create(size_t _typesize, size_t _capacity);

MEMPOOL_EXPORT void MEMPOOL_API
mempool_release(mempool _mempool);

MEMPOOL_EXPORT void* MEMPOOL_API
mempool_new(mempool _mempool);

MEMPOOL_EXPORT void MEMPOOL_API
mempool_delete(mempool _mempool, void* _alloc);

#endif