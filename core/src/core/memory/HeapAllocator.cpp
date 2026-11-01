#include <emper/core/memory/HeapAllocator.hpp>

#include <new> //new, delete

void* emper::HeapAllocator::allocate(emper::size_t size, emper::size_t alignment)
{
    // Note: alignment? later
    return ::operator new(size);
}

void emper::HeapAllocator::deallocate(void* ptr) noexcept
{
    ::operator delete(ptr);
}