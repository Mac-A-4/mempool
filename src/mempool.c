#include <mempool.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdbool.h>

static inline void*
mempool_malloc(size_t _size) {
    return malloc(_size);
}

static inline void
mempool_free(void* _alloc) {
    free(_alloc);
}

typedef struct _mempool_node {
    struct _mempool_node*
        forward;
} mempool_node;

struct _mempool {
    mempool_node
        *first_node,
        *first_region;
    size_t
        capacity,
        typesize;
};

#define NODE_AT(N) \
    ((mempool_node*)((char*)(region + 1) + (sizeof(mempool_node) + _mempool->typesize) * (N)))

static bool
mempool_new_region(mempool _mempool, size_t _size) {
    mempool_node* region = (mempool_node*)mempool_malloc(
        sizeof(mempool_node) + (sizeof(mempool_node) + _mempool->typesize) * _size
    );
    if (!region) {
        return false;
    }
    region->forward = _mempool->first_region;
    for (size_t i = 1; i < _size; ++i) {
        NODE_AT(i - 1)->forward = NODE_AT(i); 
    }
    NODE_AT(_size - 1)->forward = _mempool->first_node;
    _mempool->first_node = NODE_AT(0);
    _mempool->first_region = region;
    _mempool->capacity += _size;
    return true;
}

#undef NODE_AT

MEMPOOL_EXPORT mempool MEMPOOL_API
mempool_create(size_t _typesize, size_t _capacity) {
    mempool result = (mempool)mempool_malloc(sizeof(struct _mempool));
    if (!result) {
        return 0;
    }
    result->typesize = _typesize;
    result->capacity = 0;
    result->first_node = 0;
    result->first_region = 0;
    if (!mempool_new_region(result, _capacity)) {
        free(result);
        return 0;
    }
    return result;
}

static void
mempool_delete_region(mempool_node* _region) {
    if (!_region) {
        return;
    }
    mempool_delete_region(_region->forward);
    mempool_free(_region);
}

MEMPOOL_EXPORT void MEMPOOL_API
mempool_release(mempool _mempool) {
    mempool_delete_region(_mempool->first_region);
    mempool_free(_mempool);
}

#define TO_ALLOC(A) \
    ((void*)((mempool_node*)A + 1))
#define TO_NODE(A) \
    ((mempool_node*)A - 1)

MEMPOOL_EXPORT void* MEMPOOL_API
mempool_new(mempool _mempool) {
    if (!_mempool->first_node && !mempool_new_region(_mempool, _mempool->capacity)) {
        return 0;
    }
    void* result = TO_ALLOC(_mempool->first_node);
    _mempool->first_node = _mempool->first_node->forward;
    return result;
}

MEMPOOL_EXPORT void MEMPOOL_API
mempool_delete(mempool _mempool, void* _alloc) {
    mempool_node* node = TO_NODE(_alloc);
    node->forward = _mempool->first_node;
    _mempool->first_node = node;
}

#undef TO_ALLOC
#undef TO_NODE