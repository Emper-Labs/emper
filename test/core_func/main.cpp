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

    std::printf("\nAll memory tests passed.\n");

    return 0;
}