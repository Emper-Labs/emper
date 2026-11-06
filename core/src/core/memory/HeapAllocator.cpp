#include <emper/core/memory/HeapAllocator.hpp>

#include <new> //new, delete

void* emper::HeapAllocator::allocate(size_t size, size_t alignment)
{
    if (alignment <= __STDCPP_DEFAULT_NEW_ALIGNMENT__)
    {
        return ::operator new(size);
    }

    return ::operator new(size, std::align_val_t(alignment));
}

void emper::HeapAllocator::deallocate(void* ptr, size_t alignment)
{
    if (!ptr)
        return;

    if (alignment <= __STDCPP_DEFAULT_NEW_ALIGNMENT__)
    {
        ::operator delete(ptr);
    }
    else
    {
        ::operator delete(ptr, std::align_val_t(alignment));
    }
}