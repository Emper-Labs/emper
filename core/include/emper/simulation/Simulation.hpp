#pragma once

#include <chrono>
#include <memory>
#include <emper/type.hpp>

#include <emper/core/memory/IMemoryManager.hpp>
#include <emper/simulation/SystemScheduler.hpp>

using Clock =  std::chrono::steady_clock;


namespace emper
{

enum class SimulationState : u8
{
    Uninitialized,
    Initialized,
    Running,
    Shutdown,
    Failed
};


struct SimulationConfig
{
};

struct SimulationContext
{   
    SimulationConfig config;

    SimulationState state = SimulationState::Uninitialized;
    
    f64 elapsedTime = 0.0;
    u64 tickCount = 0;
    Clock::time_point lastTick;
};


class Simulation
{
public:
    ~Simulation() = default;
    
    Simulation(SimulationConfig config = SimulationConfig());
    Simulation(std::unique_ptr<emper::IMemoryManager> memoryManager,SimulationConfig config = SimulationConfig());
    
    bool initialize();
    
    void tick(f64 dt);
    void tick();

    void shutdown();

    bool running() const noexcept{
        return m_context.state == SimulationState::Initialized 
        || m_context.state == SimulationState::Running;
    }

    const SimulationContext& context() const{
        return m_context;
    };
    
    const IMemoryManager& memoryManager() const
    {
        return *m_memoryManager;
    }
private:
    SimulationContext m_context;
    SystemContext m_systemContext;
    
    emper::SystemScheduler m_systemScheduler;

    std::unique_ptr<emper::IMemoryManager> m_memoryManager;
};

}