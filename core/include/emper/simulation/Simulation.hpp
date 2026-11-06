#pragma once

#include <memory>


#include <emper/type.hpp>
#include <emper/core/memory/MemoryManager.hpp>

namespace emper
{

class Simulation
{
public:
    ~Simulation() = default;
    
    Simulation();
    Simulation(std::unique_ptr<emper::IMemoryManager> m_memoryManager);
    

    IMemoryManager& memoryManager() const
    {
        return *m_memoryManager;
    }


private:
    std::unique_ptr<emper::IMemoryManager> m_memoryManager;
};

}