#pragma once

#include <memory>


#include <emper/core/type.hpp>
#include <emper/core/IAllocator.hpp>

namespace emper
{

class Simulation
{
public:
    ~Simulation() = default;
    Simulation(std::unique_ptr<emper::IAllocator> allocator)    
    : m_allocator(std::move(allocator))
    {
    }

    IAllocator* getAllocator() const
    {
        return m_allocator.get();
    }


private:
    std::unique_ptr<emper::IAllocator> m_allocator;
};

}