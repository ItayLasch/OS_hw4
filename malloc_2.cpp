#include "utills.h"
#define META_DATA_SIZE sizeof(MallocMetadata)

MallocSystem m_system = CreateMallocSystem();

void *smalloc(size_t size)
{
    if (size == 0 || size > 100000000)
    {
        return NULL;
    }
    Metadata tmp = m_system->head;
    while (tmp)
    {
        if (tmp->is_free || tmp->size >= size)
        {
            tmp->is_free = false;
            m_system->num_free_blocks--;
            m_system->num_free_bytes -= size;
            return (void *)(tmp + META_DATA_SIZE);
        }
        tmp = tmp->next;
    }

    void *res = AddBlock(m_system, size, META_DATA_SIZE);
    if (res != NULL)
    {
        m_system->num_total_blocks++;
        m_system->num_total_bytes += size;
    }
    return res;
}

void *scalloc(size_t num, size_t size)
{
    void *new_block = smalloc(size * num);
    if (new_block == nullptr)
    {
        return nullptr;
    }
    memset(new_block, 0, size * num);
    return new_block;
}

void sfree(void *p)
{
    if (p == nullptr)
    {
        return;
    }
    Metadata block_to_free = (Metadata)((int *)p - META_DATA_SIZE);
    freeBlock(block_to_free);
    if (block_to_free->is_free)
    {
        return;
    }
    m_system->num_free_blocks++;
    m_system->num_free_bytes += block_to_free->size;
}

void *srealloc(void *oldp, size_t size)
{
    if (size == 0 || size > 100000000)
    {
        return nullptr;
    }
    if (oldp == nullptr)
    {
        return smalloc(size);
    }
    Metadata block_to_free = (Metadata)((int *)oldp - META_DATA_SIZE);
    if (size <= block_to_free->size)
    {
        return oldp;
    }
    void *newp = smalloc(size);
    if (newp == nullptr)
    {
        return nullptr;
    }
    std::memmove(newp, oldp, size);
    sfree(oldp);
    return newp;
}


size_t _num_free_blocks()
{
    return m_system->num_free_blocks;
}

size_t _num_free_bytes()
{
    return m_system->num_free_bytes;
}

size_t _num_allocated_blocks()
{
    return m_system->num_total_blocks;
}

size_t _num_allocated_bytes()
{
    return m_system->num_total_bytes;
}

size_t _num_meta_data_bytes()
{
    return m_system->num_total_blocks * META_DATA_SIZE;
}

size_t _size_meta_data()
{
    return META_DATA_SIZE;
}

int main()
{
    return 0;
}