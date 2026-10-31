#include <emper/core/HeapAllocator.hpp>

#include <new> //new, delete

void* HeapAllocator::allocate(emper::size_t size)
{
    return ::operator new(size);
}

void HeapAllocator::deallocate(void* ptr)
{
    ::operator delete(ptr);
}