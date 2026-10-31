#include <emper/core/HeapAllocator.hpp>

#include <new> //new, delete

void* emper::HeapAllocator::allocate(emper::size_t size)
{
    return ::operator new(size);
}

void emper::HeapAllocator::deallocate(void* ptr)
{
    ::operator delete(ptr);
}