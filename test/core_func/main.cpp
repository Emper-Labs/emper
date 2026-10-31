#include <emper/simulation/Simulation.hpp>

#include <emper/core/HeapAllocator.hpp>

#include <cstring> //std::memcpy

void testHeapAlloc()
{
    auto allocator = std::make_unique<emper::HeapAllocator>();
    emper::Simulation simulation(std::move(allocator));
    
    
    
    void* memory = simulation.allocator().allocate(1024);
    
    std::memcpy(memory, "Hello, Emper!", 14);
    std::printf("%s\n", static_cast<char*>(memory));

    
    simulation.allocator().deallocate(memory);


}


int main()
{
    testHeapAlloc();
    return 0;
}