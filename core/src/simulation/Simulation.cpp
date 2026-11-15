#include <emper/core/memory/DefaultMemoryManager.hpp>
#include <emper/simulation/Simulation.hpp>

namespace emper
{

Simulation::Simulation(SimulationConfig config) 
: m_memoryManager(std::make_unique<emper::DefaultMemoryManager>()), m_context() {
    m_context.state = SimulationState::Created;
    m_context.config = config;
}

Simulation::Simulation(std::unique_ptr<emper::IMemoryManager> memoryManager, SimulationConfig config) 
: m_memoryManager(std::move(memoryManager)), m_context() {
    m_context.state = SimulationState::Created;
    m_context.config = config;
}

bool Simulation::initialize() {
    // Initialize simulation resources here
    m_context.state = SimulationState::Initialized;
    return true;
}

void Simulation::tick(f64 dt) {
    // Update simulation state based on the time step dt
}


void Simulation::shutdown() {
    // Clean up simulation resources here
    m_context.state = SimulationState::Shutdown;
}

}

