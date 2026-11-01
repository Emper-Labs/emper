#pragma once

#include <emper/type.hpp>

namespace emper
{

class IAllocator{
public:
    IAllocator() = default;
    virtual ~IAllocator() = default;

    virtual void* allocate(emper::size_t size, emper::size_t alignment) = 0;
    virtual void deallocate(void* ptr) = 0;
};

}