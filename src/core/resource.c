#include "mc/resource.h"
#include "mc/utils.h"
#include <stddef.h>

void *mc_resource_acquire(const mc_resource_manager_t *mgr, int32_t key)
{
    // 1. Check if key already exists
    void *ret = mc_resource_get(mgr, key);
    if (ret)
    {
        return ret;
    }

    // 2. Scan for the first free slot
    for (int i = 0; i < mgr->count; i++)
    {
        if (!mgr->pool[i].is_busy)
        {
            mgr->pool[i].is_busy = true;
            mgr->pool[i].key = key;
            return mgr->pool[i].data;
        }
    }

    // 3. Pool is full. Return NULL.
    return NULL;
}

void *mc_resource_get(const mc_resource_manager_t *mgr, int32_t key)
{
    MC_ASSERT(mgr != NULL);
    for (int i = 0; i < mgr->count; i++)
    {
        if (mgr->pool[i].is_busy && mgr->pool[i].key == key)
        {
            return mgr->pool[i].data;
        }
    }
    return NULL;
}

void mc_resource_release(const mc_resource_manager_t *mgr, void *data)
{
    MC_ASSERT(mgr != NULL);
    for (int i = 0; i < mgr->count; i++)
    {
        if (mgr->pool[i].data == data)
        {
            mgr->pool[i].is_busy = false;
            return;
        }
    }
}

void mc_resource_release_key(const mc_resource_manager_t *mgr, int32_t key)
{
    MC_ASSERT(mgr != NULL);
    for (int i = 0; i < mgr->count; i++)
    {
        if (mgr->pool[i].is_busy && mgr->pool[i].key == key)
        {
            mgr->pool[i].is_busy = false;
            return;
        }
    }
}