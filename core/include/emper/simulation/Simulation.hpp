#pragma once

#include <memory>


#include <emper/type.hpp>
#include <emper/core/memory/IAllocator.hpp>

namespace emper
{

class Simulation
{
public:
    ~Simulation() = default;
    Simulation(std::unique_ptr<emper::IAllocator> allocator);

    IAllocator& allocator() const
    {
        return *m_allocator;
    }


private:
    std::unique_ptr<emper::IAllocator> m_allocator;
};

}