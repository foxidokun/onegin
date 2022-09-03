#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "onegin.h"
#include "hashmap.h"

hashmap *hashmap_create (size_t capacity, size_t key_size, size_t val_size,
                    long unsigned int hash(const void *), int (*comp)(const void *, const void *))
{
    assert (hash != NULL && "function can't be NULL");

    hashmap *map   = (hashmap *) calloc (1, sizeof (hashmap));
    _UNWRAP_NULL (map);

    map->used      = 0;
    map->allocated = capacity;
    map->key_size  = key_size;
    map->val_size  = val_size;
    map->flags     = create_bitflags (capacity);
    map->keys      = calloc (capacity, key_size);
    map->values    = calloc (capacity, val_size);
    map->hash      = hash;
    map->comp      = comp;
    
    _UNWRAP_NULL (map->flags);
    _UNWRAP_NULL (map->keys);
    _UNWRAP_NULL (map->values);

    return map;
}

void hashmap_free (hashmap *map)
{
    free_bitflags (map->flags);
    free (map->keys);
    free (map->values);

    free (map);
}

void hashmap_clear (hashmap *map)
{
    assert (map != NULL && "pointer can't be NULL");

    map->used = 0;
    clear_bitflags (map->flags);
}

hashmap *hashmap_resize (hashmap *old_map, size_t new_size)
{
    assert (new_size > old_map->used && "New hashmap must be bigger than number of elements in old");

    hashmap *new_map = hashmap_create (new_size, old_map->key_size, old_map->val_size,
                                        old_map->hash, old_map->comp);
    _UNWRAP_NULL (new_map);


    for (size_t id = 0; id < old_map->allocated; ++id)
    {
        if (check_bit (old_map->flags, id))
        {
            hashmap_insert (new_map, (char *) old_map->keys   + id*old_map->key_size,
                                     (char *) old_map->values + id*old_map->val_size);
        }
    }

    hashmap_free (old_map);

    return new_map;
}

int hashmap_insert (hashmap *map, const void *key, const void *value)
{
    if (map->allocated == map->used) return ERROR; // OOM

    size_t id = map->hash (key) % map->allocated;

    if (check_bit (map->flags, id))
    {
        ssize_t id_tmp = bit_find_value (map->flags, 0);
        assert (id_tmp != ERROR && "Used < allocated, but all cells are occupied");

        id = (size_t) id_tmp;
    }

    set_bit_true (map->flags, id);
    map->used++;

    memcpy (map->keys,   key,   map->key_size);
    memcpy (map->values, value, map->val_size);

    return 0;
}

const void *hashmap_get (const hashmap *map, const void *key)
{
    size_t id = map->hash (key) % map->allocated;

    if (!map->comp (key, (char *) map->keys + id*map->key_size))
    {
        return (char *) map->values + id*map->val_size;
    }

    for (id = 0; id < map->allocated; ++id)
    {
        if (check_bit (map->flags, id) && !map->comp (key, (char *) map->keys + id*map->key_size))
        {
            return (char *) map->values + id*map->val_size;
        }
    }

    return NULL;
}