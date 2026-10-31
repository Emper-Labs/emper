#pragma once

#include "type.hpp"

namespace emper
{

class IAllocator{
public:
    IAllocator() = default;
    virtual ~IAllocator() = default;

    virtual void* allocate(emper::size_t size) = 0;
    virtual void deallocate(void* ptr) = 0;
};

}