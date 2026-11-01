#pragma once

#include "IAllocator.hpp"

namespace emper
{

class HeapAllocator final : public IAllocator
{
public:
    void* allocate(emper::size_t size, emper::size_t alignment) override;

    void deallocate(void* ptr) override;
};

}