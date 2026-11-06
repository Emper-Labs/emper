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
    
    void* ptr = simulation.memoryManager().mapBuffer(buf);
    if (ptr)
    {
        const char* testData = "Hello, Emper!";
        std::memcpy(ptr, testData, std::strlen(testData) + 1);

        std::printf("Buffer content: %s\n", static_cast<const char*>(ptr));


        simulation.memoryManager().unmapBuffer(buf);
    }
}


int main()
{
    testHeapAlloc();
    return 0;
}