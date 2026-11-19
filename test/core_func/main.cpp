#include <cstdio>

// HeapAllocator tests
void testBasicAllocation();
void testAlignment();
void testWriteRead();
void testMultipleAllocations();
void testInvalidAlignment();
void testZeroSizeAllocation();
void testNullDeallocation();

// HostBufferResource tests
void testHostBufferResourceCreation();
void testHostBufferResourceWriteRead();
void testHostBufferResourceAlignment();
void testHostBufferResourceDestruction();

//LifeCycle tests
void testSimulationLifeCycle();


int main()
{
    // HeapAllocator
    std::printf("\nHeapAllocator tests:\n");
    testBasicAllocation();
    testAlignment();
    testWriteRead();
    testMultipleAllocations();
    testInvalidAlignment();
    testZeroSizeAllocation();
    testNullDeallocation();
    // HostBufferResource
    std::printf("\nHostBufferResource tests:\n");
    testHostBufferResourceCreation();
    testHostBufferResourceWriteRead();
    testHostBufferResourceAlignment();
    testHostBufferResourceDestruction();
    
    // LifeCycle
    std::printf("\nSimulation LifeCycle tests:\n");
    testSimulationLifeCycle();

    return 0;
}