#include <emper/simulation/Simulation.hpp>

#include <cassert>

void testSimulationLifeCycle() {
    emper::Simulation simulation;

    // Arrange
    constexpr emper::u32 MAX_TICKS = 10;

    // Act: Initialize
    simulation.initialize();

    assert(simulation.running());
    assert(simulation.context().state == emper::SimulationState::Initialized);

    // Act: Run simulation
    for (emper::u32 i = 0; i < MAX_TICKS && simulation.running(); ++i) {
        simulation.tick(0.016);
    }

    assert(simulation.context().state == emper::SimulationState::Running);
    assert(simulation.context().tickCount == MAX_TICKS);

    // Act: Shutdown
    simulation.shutdown();

    // Assert
    assert(!simulation.running());
    assert(simulation.context().state == emper::SimulationState::Shutdown);

    std::printf("[PASS] SimulationLifeCycle\n");
}