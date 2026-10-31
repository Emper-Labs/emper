#include <emper/simulation/Simulation.hpp>


emper::Simulation::Simulation(std::unique_ptr<emper::IAllocator> allocator)
    : m_allocator(std::move(allocator))
{
    if (!m_allocator)
    {
        throw std::invalid_argument("Allocator cannot be null");
    }
}


