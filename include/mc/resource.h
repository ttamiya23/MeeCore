#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define MC_RESOURCE_ENTRY(DATA) { \
    .data = &DATA,                \
    .key = 0,                     \
    .is_busy = false}

#define MC_DEFINE_RESOURCE_MANAGER(NAME, POOL, COUNT) \
    const mc_resource_manager_t NAME = {              \
        .pool = POOL,                                 \
        .count = COUNT}

    // Wrapper for a managed resource.
    typedef struct mc_resource_entry_t
    {
        void *data;
        int32_t key;
        bool is_busy;
    } mc_resource_entry_t;

    // The Resource Manager control structure.
    typedef struct mc_resource_manager_t
    {
        mc_resource_entry_t *pool;
        uint8_t count;
    } mc_resource_manager_t;

    /**
     * Request a resource for a specific key.
     * * Logic:
     * 1. If 'key' is already active, return that resource.
     * 2. If there is a free resource, claim it and return it.
     * 3. If full, return NULL (request ignored).
     */
    void *mc_resource_acquire(const mc_resource_manager_t *mgr, int32_t key);

    // Find the resource currently assigned to this key.
    void *mc_resource_get(const mc_resource_manager_t *mgr, int32_t key);

    // Release the specific resource.
    void mc_resource_release(const mc_resource_manager_t *mgr, void *resource);

    // Release whatever resource is holding this key.
    void mc_resource_release_key(const mc_resource_manager_t *mgr, int32_t key);

#ifdef __cplusplus
}
#endif