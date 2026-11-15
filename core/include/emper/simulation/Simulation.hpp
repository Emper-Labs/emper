#pragma once

#include <memory>


#include <emper/type.hpp>
#include <emper/core/memory/IMemoryManager.hpp>

namespace emper
{

enum class SimulationState : u8
{
    Created,
    Initialized,
    Shutdown,
    Failed
};


struct SimulationConfig
{
};

struct SimulationContext
{
    SimulationState state = SimulationState::Created;
    
    SimulationConfig config;
};


class Simulation
{
public:
    ~Simulation() = default;
    
    Simulation(SimulationConfig config = SimulationConfig());
    Simulation(std::unique_ptr<emper::IMemoryManager> memoryManager,SimulationConfig config = SimulationConfig());
    



    bool initialize();
    void tick(f64 dt);
    void shutdown();

    const SimulationContext& context() const{
        return m_context;
    };
    
    const IMemoryManager& memoryManager() const
    {
        return *m_memoryManager;
    }
private:
    SimulationContext m_context;
    std::unique_ptr<emper::IMemoryManager> m_memoryManager;    
};

}