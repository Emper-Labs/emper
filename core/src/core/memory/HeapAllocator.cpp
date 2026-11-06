#include <emper/core/memory/HeapAllocator.hpp>

#include <new>
#include <stdexcept>

void* emper::HeapAllocator::allocate(size_t size, size_t alignment)
{
    if (size == 0)
    {
        throw std::invalid_argument("Allocation size cannot be zero");
    }

    if (alignment == 0 || (alignment & (alignment - 1)) != 0)
    {
        throw std::invalid_argument("Alignment must be a power of two");
    }

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
        
    if (alignment == 0 || (alignment & (alignment - 1)) != 0)
    {
        //no except
        //throw std::invalid_argument("Alignment must be a power of two");
        return;
    }


    if (alignment <= __STDCPP_DEFAULT_NEW_ALIGNMENT__)
    {
        ::operator delete(ptr);
    }
    else
    {
        ::operator delete(ptr, std::align_val_t(alignment));
    }
}