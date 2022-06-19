#ifndef UTILLS_H
#define UTILLS_H

#include <unistd.h>
#include <stdbool.h>
#include <iostream>
#include <cstring>

struct MallocMetadata
{
    size_t size;
    bool is_free;
    MallocMetadata *next;
    MallocMetadata *prev;
};

struct MallocSystem_t
{
    MallocMetadata *head;
    size_t num_free_blocks;
    size_t num_free_bytes;
    size_t num_total_blocks;
    size_t num_total_bytes;
};

typedef struct MallocMetadata *Metadata;
typedef struct MallocSystem_t *MallocSystem;

MallocSystem CreateMallocSystem()
{
    MallocSystem ms = (MallocSystem)sbrk(sizeof(*ms));
    ms->head = NULL;
    ms->num_free_blocks = 0;
    ms->num_free_bytes = 0;
    ms->num_total_blocks = 0;
    ms->num_total_bytes = 0;

    return ms;
}

bool isEmpty(MallocSystem ms)
{
    if (ms->head == NULL)
    {
        return true;
    }

    return false;
}

Metadata newNode(size_t size, size_t meta_data_size)
{
    void *res = sbrk(size + meta_data_size);
    if (res == (void *)-1)
    {
        return nullptr;
    }

    Metadata node = (Metadata)res;
    node->size = size;
    node->is_free = false;
    node->next = nullptr;
    node->prev = nullptr;
    return node;
}

void *AddBlock(MallocSystem ms, size_t size, size_t meta_data_size)
{
    Metadata new_malloc = (Metadata)sbrk(size + meta_data_size);
    if (new_malloc == nullptr)
    {
        return nullptr;
    }

    if (isEmpty(ms)) // in case list is empty
    {
        ms->head = new_malloc;
    }
    else if (new_malloc->size < ms->head->size) // in case new malloc is the smallest
    {
        Metadata tmp = ms->head;
        ms->head = new_malloc;
        ms->head->next = tmp;
    }
    else // find new malloc location in list
    {
        Metadata tmp = ms->head;
        while (tmp->next && tmp->size < new_malloc->size)
        {
            tmp = tmp->next;
        }

        if(tmp->size < new_malloc->size)
        {
            new_malloc->prev = tmp;
            new_malloc->next = tmp->next;
            tmp->next = new_malloc;
        }
        else
        {
            new_malloc->next = tmp;
            new_malloc->prev = tmp->prev;
            tmp->prev = new_malloc;
        }
    }

    return (new_malloc + meta_data_size);
}

void freeBlock(Metadata remove)
{
    if(remove == nullptr)
    {
        return;
    }

    remove->is_free = true;
}

void *smalloc(size_t size);
void *scalloc(size_t num, size_t size);
void sfree(void *p);
void *srealloc(void *oldp, size_t size);
size_t _num_free_blocks();
size_t _num_free_bytes();
size_t _num_allocated_blocks();
size_t _num_allocated_bytes();
size_t _num_meta_data_bytes();
size_t _size_meta_data();

#endif // UTILLS_H