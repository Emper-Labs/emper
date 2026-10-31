#include <emper/simulation/Simulation.hpp>

#include <emper/core/HeapAllocator.hpp>


void testHeapAlloc()
{
    auto allocator = std::make_unique<emper::HeapAllocator>();
    emper::Simulation simulation(std::move(allocator));
    void* memory = simulation.getAllocator()->allocate(1024);
    simulation.getAllocator()->deallocate(memory);
}


int main()
{
    testHeapAlloc();
    return 0;
}