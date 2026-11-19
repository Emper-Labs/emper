#include <emper/simulation/SystemScheduler.hpp>

namespace emper {

void SystemScheduler::addSystem(std::unique_ptr<ISystem> system) {
    if(!system)
        return;

    m_systems.push_back(std::move(system));
}

bool SystemScheduler::initialize(SystemContext& context) {
    m_context.m_initializedCount = 0;

    for (auto& system : m_systems) {
        if (!system->initialize(context)) {
            // Initialization failed: rollback
            while (m_context.m_initializedCount > 0) {
                --m_context.m_initializedCount;
                m_systems[m_context.m_initializedCount]->shutdown(context);
            }

            return false;
        }

        ++m_context.m_initializedCount;
    }

    return true;
}

void SystemScheduler::update(SystemContext& context, f64 dt) {
    for (size_t i = 0; i < m_context.m_initializedCount; ++i) {
        m_systems[i]->update(context, dt);
    }
}

void SystemScheduler::shutdown(SystemContext& context) {
    while (m_context.m_initializedCount > 0) {
        --m_context.m_initializedCount;
        m_systems[m_context.m_initializedCount]->shutdown(context);
    }
}

}