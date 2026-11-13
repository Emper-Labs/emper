#include <emper/core/memory/HeapAllocator.hpp>

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <new>
#include <stdexcept>
#include <limits>

using namespace emper;

// --------------------------------------------------
// Test utilities
// --------------------------------------------------

template<typename Exception, typename Func>
void expectThrow(Func&& func)
{
    bool thrown = false;

    try
    {
        func();
    }
    catch (const Exception&)
    {
        thrown = true;
    }

    assert(thrown);
}

bool isBufferAligned(const void* ptr, size_t alignment)
{
    return reinterpret_cast<std::uintptr_t>(ptr) % alignment == 0;
}

// --------------------------------------------------
// Test 1: Basic allocation
// --------------------------------------------------

void testBasicAllocation()
{
    HeapAllocator allocator;

    constexpr size_t size = 1024;
    constexpr size_t alignment = alignof(std::max_align_t);

    void* ptr = allocator.allocate(size, alignment);

    assert(ptr != nullptr);
    assert(isBufferAligned(ptr, alignment));

    allocator.deallocate(ptr, alignment);

    std::printf("[PASS] Basic Allocation\n");
}

// --------------------------------------------------
// Test 2: Alignment
// --------------------------------------------------

void testAlignment()
{
    HeapAllocator allocator;

    constexpr size_t size = 1024;

    constexpr size_t alignments[] = {
        8, 16, 32, 64, 128, 256, 512, 1024
    };

    for (size_t alignment : alignments)
    {
        void* ptr = allocator.allocate(size, alignment);

        assert(ptr != nullptr);
        assert(isBufferAligned(ptr, alignment));

        allocator.deallocate(ptr, alignment);
    }

    std::printf("[PASS] Alignment (8 - 1024 bytes)\n");
}

// --------------------------------------------------
// Test 3: Write / Read
// --------------------------------------------------

void testWriteRead()
{
    HeapAllocator allocator;

    constexpr size_t size = 1024;
    constexpr size_t alignment = 64;

    void* ptr = allocator.allocate(size, alignment);

    const char* message = "Hello, Emper!";

    std::memcpy(
        ptr,
        message,
        std::strlen(message) + 1
    );

    assert(std::strcmp(
        static_cast<const char*>(ptr),
        message
    ) == 0);

    allocator.deallocate(ptr, alignment);

    std::printf("[PASS] Write / Read\n");
}

// --------------------------------------------------
// Test 4: Multiple allocations
// --------------------------------------------------

void testMultipleAllocations()
{
    HeapAllocator allocator;

    constexpr size_t count = 100;
    constexpr size_t size = 256;
    constexpr size_t alignment = 64;

    void* pointers[count] = {};

    for (size_t i = 0; i < count; ++i)
    {
        pointers[i] = allocator.allocate(size, alignment);

        assert(pointers[i] != nullptr);
        assert(isBufferAligned(pointers[i], alignment));
    }

    for (size_t i = 0; i < count; ++i)
    {
        allocator.deallocate(pointers[i], alignment);
    }

    std::printf("[PASS] Multiple Allocations\n");
}

// --------------------------------------------------
// Test 5: Invalid alignment
// --------------------------------------------------

void testInvalidAlignment()
{
    HeapAllocator allocator;

    expectThrow<std::invalid_argument>(
        [&]()
        {
            allocator.allocate(128, 0);
        }
    );

    expectThrow<std::invalid_argument>(
        [&]()
        {
            allocator.allocate(128, 3);
        }
    );

    std::printf("[PASS] Invalid Alignment Detection\n");
}

// --------------------------------------------------
// Test 6: Zero-size allocation
// --------------------------------------------------

void testZeroSizeAllocation()
{
    HeapAllocator allocator;

    expectThrow<std::invalid_argument>(
        [&]()
        {
            allocator.allocate(0, 16);
        }
    );

    std::printf("[PASS] Zero Size Detection\n");
}

// --------------------------------------------------
// Test 7: Null deallocation
// --------------------------------------------------

void testNullDeallocation()
{
    HeapAllocator allocator;

    // Deallocating nullptr should be safe.
    allocator.deallocate(nullptr, 16);

    std::printf("[PASS] Null Deallocation\n");
}