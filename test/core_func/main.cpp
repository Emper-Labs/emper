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

// Storage tests
void testTypedStorageCreation();
void testMultipleInstances();
void testDestroyStorage();
void testInvalidAndStaleHandles();
void testCapacityValidation();
void testLogicalCount();
void testSoAFieldAccess();
void testFieldIsolation();
void testReadWriteViews();
void testRejectSimultaneousViews();
void testRejectDestroyWithActiveView();
void testAllocationRollback();
void testMemoryAlignment();
void testMismatchedLayoutHandle();


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

    // Storage
    std::printf("\nStorage tests:\n");
    testTypedStorageCreation();
    testMultipleInstances();
    testDestroyStorage();
    testInvalidAndStaleHandles();
    testCapacityValidation();
    testLogicalCount();
    testSoAFieldAccess();
    testFieldIsolation();
    testReadWriteViews();
    testRejectSimultaneousViews();
    testRejectDestroyWithActiveView();
    testAllocationRollback();
    testMemoryAlignment();
    testMismatchedLayoutHandle();

    return 0;
}