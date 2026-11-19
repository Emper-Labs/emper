#include <emper/simulation/SystemScheduler.hpp>

namespace emper {

void SystemScheduler::addSystem(std::unique_ptr<ISystem> system) {
    if (!system || m_context.state != SystemSchedulerState::Uninitialized)
        return;

    m_systems.push_back(std::move(system));
}

bool SystemScheduler::initialize(SystemContext& context) {
    if (m_context.state != SystemSchedulerState::Uninitialized)
        return false;

    m_context.initializedCount = 0;

    for (auto& system : m_systems) {
        if (!system->initialize(context)) {
            // Initialization failed: rollback
            while (m_context.initializedCount > 0) {
                --m_context.initializedCount;
                m_systems[m_context.initializedCount]->shutdown(context);
            }

            m_context.state = SystemSchedulerState::Failed;
            return false;
        }

        ++m_context.initializedCount;
    }

    m_context.state = SystemSchedulerState::Initialized;
    return true;
}

void SystemScheduler::update(SystemContext& context, f64 dt) {
    if (m_context.state != SystemSchedulerState::Initialized)
        return;

    for (size_t i = 0; i < m_context.initializedCount; ++i) {
        m_systems[i]->update(context, dt);
    }
}

void SystemScheduler::shutdown(SystemContext& context) {
    if (m_context.state == SystemSchedulerState::Shutdown ||
        m_context.state == SystemSchedulerState::Uninitialized)
        return;

    while (m_context.initializedCount > 0) {
        --m_context.initializedCount;
        m_systems[m_context.initializedCount]->shutdown(context);
    }

    m_context.state = SystemSchedulerState::Shutdown;
}

}