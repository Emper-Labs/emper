#pragma once

#include <vector>
#include <memory>
#include <emper/simulation/ISystem.hpp>

namespace emper {

struct SystemSchedulerContext
{
    u32 m_initializedCount = 0;
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