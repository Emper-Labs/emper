#pragma once

#include <memory>


#include "type.hpp"

#include "core/IAllocator.hpp"

namespace emper
{

class Simulation
{
public:
    ~Simulation() = default;
    Simulation(std::unique_ptr<emper::IAllocator> allocator);
private:
    std::unique_ptr<emper::IAllocator> m_allocator;


};

}