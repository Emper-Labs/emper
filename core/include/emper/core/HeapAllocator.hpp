#pragma once

#include "IAllocator.hpp"

class HeapAllocator final : public IAllocator
{
public:
    void* allocate(emper::size_t size) override;

    void deallocate(void* ptr) override;
};