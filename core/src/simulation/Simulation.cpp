#include <emper/core/memory/DefaultMemoryManager.hpp>
#include <emper/simulation/Simulation.hpp>

namespace emper
{

Simulation::Simulation() 
: m_memoryManager(std::make_unique<emper::DefaultMemoryManager>()) {

}

Simulation::Simulation(std::unique_ptr<emper::IMemoryManager> memoryManager) 
: m_memoryManager(std::move(memoryManager)) {
    
}

}

