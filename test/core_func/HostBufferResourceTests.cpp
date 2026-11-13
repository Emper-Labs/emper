#include <emper/core/memory/HostBufferResource.hpp>
#include <emper/core/memory/HeapAllocator.hpp>

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <memory>

using namespace emper;

bool isAligned(const void* ptr, size_t alignment)
{
    const auto address = reinterpret_cast<std::uintptr_t>(ptr);
    return address % alignment == 0;
}

// Test 1: Creation
void testHostBufferResourceCreation()
{
    HeapAllocator allocator;

    HostBufferResource resource(allocator, 1024, 64);

    assert(resource.size() == 1024);
    assert(resource.data() != nullptr);

    std::printf("[PASS] HostBufferResource Creation\n");
}

// Test 2: Write / Read
void testHostBufferResourceWriteRead()
{
    HeapAllocator allocator;

    HostBufferResource resource(allocator, 1024, 64);

    const char* message = "Hello, Emper!";

    std::memcpy(
        resource.data(),
        message,
        std::strlen(message) + 1
    );

    assert(std::strcmp(
        static_cast<const char*>(resource.data()),
        message
    ) == 0);

    std::printf("[PASS] HostBufferResource Write / Read\n");
}

// Test 3: Alignment
void testHostBufferResourceAlignment()
{
    HeapAllocator allocator;

    constexpr size_t alignments[] = {
        8, 16, 32, 64, 128, 256, 512, 1024
    };

    for (size_t alignment : alignments)
    {
        HostBufferResource resource(
            allocator,
            1024,
            alignment
        );

        assert(resource.data() != nullptr);
        assert(isAligned(resource.data(), alignment));
    }

    std::printf("[PASS] HostBufferResource Alignment\n");
}

// Test 4: Destruction through unique_ptr
void testHostBufferResourceDestruction()
{
    HeapAllocator allocator;

    {
        std::unique_ptr<IBufferResource> resource =
            std::make_unique<HostBufferResource>(
                allocator,
                1024,
                64
            );

        assert(resource != nullptr);
        assert(resource->size() == 1024);
    }

    // Resource destructor should release allocation.
    std::printf("[PASS] HostBufferResource Destruction\n");
}