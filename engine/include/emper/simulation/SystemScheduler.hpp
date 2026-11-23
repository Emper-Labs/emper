#pragma once

#include <vector>
#include <memory>
#include <emper/simulation/ISystem.hpp>

namespace emper {

enum class SystemSchedulerState : u8 {
    Uninitialized,
    Initialized,
    Failed,
    Shutdown
};

struct SystemSchedulerContext {
    SystemSchedulerState state = SystemSchedulerState::Uninitialized;
    u32 initializedCount = 0;
};

class SystemScheduler {
public:
    SystemScheduler() = default;
    ~SystemScheduler() = default;

    void addSystem(std::unique_ptr<ISystem> system);

    bool initialize(SystemContext& context);
    void update(SystemContext& context, f64 dt);
    void shutdown(SystemContext& context);

    const SystemSchedulerContext& context() const {
        return m_context;
    }
    
private:
    SystemSchedulerContext m_context;

    std::vector<std::unique_ptr<ISystem>> m_systems;
};

}