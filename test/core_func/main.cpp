#include <emper/simulation/Simulation.hpp>

#include <emper/core/memory/HeapAllocator.hpp>

#include <emper/core/memory/Buffer.hpp>

#include <cstring> //std::memcpy

void testHeapAlloc()
{
    emper::Simulation simulation;
    
    emper::Buffer buf = simulation.memoryManager().createBuffer({
        1024,
        alignof(std::max_align_t),
        emper::MemoryDomain::Host,
        emper::BufferUsage::None
    });
    

}


int main()
{
    testHeapAlloc();
    return 0;
}