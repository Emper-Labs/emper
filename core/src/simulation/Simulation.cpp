#include <cmath>

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
    m_context.lastTick = Clock::now();
    m_context.state = SimulationState::Initialized;
    
    return true;
}

void Simulation::tick(f64 dt) {
    if (!running())
        return;

    if (!std::isfinite(dt) || dt < 0.0)
        return;

    if (m_context.state == SimulationState::Initialized)
        m_context.state = SimulationState::Running;

    m_context.elapsedTime += dt;
    ++m_context.tickCount;

    // Update systems
}

void Simulation::tick(){
    const auto now = Clock::now();

    const f64 dt =
        std::chrono::duration<f64>(now - m_context.lastTick).count();

    m_context.lastTick = now;

    tick(dt);
}

void Simulation::shutdown() {
    // Clean up simulation resources here
    m_context.state = SimulationState::Shutdown;
}

}

