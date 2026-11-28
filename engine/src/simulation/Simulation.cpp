#include <cmath>

#include <emper/core/memory/DefaultMemoryManager.hpp>
#include <emper/simulation/Simulation.hpp>

namespace emper {

Simulation::Simulation(SimulationConfig config)
    : m_memoryManager(std::make_unique<DefaultMemoryManager>()),
        m_storage(std::make_unique<Storage>(*m_memoryManager)),
      m_context()
{
    m_context.config = config;
}

Simulation::Simulation(
    std::unique_ptr<IMemoryManager> memoryManager,
    SimulationConfig config
)
    : m_memoryManager(std::move(memoryManager)),
      m_storage(std::make_unique<Storage>(*m_memoryManager)),
      m_context()
{
    m_context.config = config;
}

bool Simulation::initialize() {
    if (m_context.state != SimulationState::Uninitialized)
        return false;

    if (!m_memoryManager) {
        m_context.state = SimulationState::Failed;
        return false;
    }

    // Initialize simulation resources
    // Initialize SystemScheduler
    if (!m_systemScheduler.initialize(m_systemContext)) {
        m_context.state = SimulationState::Failed;
        return false;
    }

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

    //m_systemScheduler.update(m_systemContext, dt);

    m_context.elapsedTime += dt;
    ++m_context.tickCount;
}

void Simulation::tick() {
    if (!running())
        return;

    const auto now = Clock::now();

    const f64 dt =
        std::chrono::duration<f64>(now - m_context.lastTick).count();

    m_context.lastTick = now;

    tick(dt);
}

void Simulation::shutdown() {
    if (m_context.state == SimulationState::Shutdown ||
        m_context.state == SimulationState::Uninitialized)
        return;

    m_systemScheduler.shutdown(m_systemContext);

    // Cleanup simulation resources

    m_context.state = SimulationState::Shutdown;
}

}