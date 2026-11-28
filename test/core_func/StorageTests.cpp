#include <cassert>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <new>
#include <stdexcept>

#include <emper/core/memory/DefaultMemoryManager.hpp>
#include <emper/core/memory/Storage.hpp>

using namespace emper;

// ---------------------------------------------------------------------------
// Test helper types
// ---------------------------------------------------------------------------

struct vec3
{
    f32 x, y, z;
};

struct Position {};
struct Velocity {};
struct Mass {};

using ParticleLayout = SoALayout<
    Field<vec3, Position>,
    Field<vec3, Velocity>,
    Field<f32, Mass>
>;

using ThreeU64Layout = SoALayout<
    Field<u64>,
    Field<u64>,
    Field<u64>>;

struct alignas(64) Aligned64
{
    u64 value;
};

using AlignLayout = SoALayout<
    Field<Aligned64>
>;

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

bool isAligned(const void* ptr, size_t alignment)
{
    return reinterpret_cast<std::uintptr_t>(ptr) % alignment == 0;
}

// ---------------------------------------------------------------------------
// A memory manager that fails a specific createBuffer call. Used to exercise
// transactional creation / rollback without real allocation pressure.
// ---------------------------------------------------------------------------

class FailingMemoryManager final : public IMemoryManager
{
public:
    FailingMemoryManager(IMemoryManager& inner, size_t failOnCall)
        : m_inner(inner), m_failOnCall(failOnCall) {}

    Buffer createBuffer(const BufferDesc& desc) override
    {
        ++m_createCalls;

        if (m_failOnCall != 0 && m_createCalls == m_failOnCall)
        {
            m_failOnCall = 0;
            throw std::bad_alloc{};
        }

        return m_inner.createBuffer(desc);
    }

    void destroyBuffer(const Buffer& buffer) override
    {
        ++m_destroyCalls;
        m_inner.destroyBuffer(buffer);
    }

    void* mapBuffer(const Buffer& buffer) override { return m_inner.mapBuffer(buffer); }
    void unmapBuffer(const Buffer& buffer) override { m_inner.unmapBuffer(buffer); }

    size_t createCalls() const noexcept { return m_createCalls; }
    size_t destroyCalls() const noexcept { return m_destroyCalls; }

private:
    IMemoryManager& m_inner;
    size_t m_failOnCall = 0;
    size_t m_createCalls = 0;
    size_t m_destroyCalls = 0;
};
// ---------------------------------------------------------------------------
// Test 1: Creating typed storage
// ---------------------------------------------------------------------------

void testTypedStorageCreation()
{
    DefaultMemoryManager memory;
    Storage storage(memory);

    StorageHandle handle = storage.create<ParticleLayout>(16);

    assert(handle.index + handle.generation != 0);
    assert(storage.capacity(handle) == 16);
    assert(storage.logicalCount(handle) == 0);

    std::printf("[PASS] Typed Storage Creation\n");
}

// ---------------------------------------------------------------------------
// Test 2: Multiple instances of the same type
// ---------------------------------------------------------------------------

void testMultipleInstances()
{
    DefaultMemoryManager memory;
    Storage storage(memory);

    StorageHandle a = storage.create<ParticleLayout>(8);
    StorageHandle b = storage.create<ParticleLayout>(16);

    assert(a.index != b.index);
    assert(storage.capacity(a) == 8);
    assert(storage.capacity(b) == 16);

    // Independent writes must not leak across records.
    {
        auto va = storage.view<ParticleLayout, Access::ReadWrite>(a);
        va.field<Position>()[0] = vec3{1.0f, 2.0f, 3.0f};
        va.field<Mass>()[0] = 99.0f;
    }
    {
        auto vb = storage.view<ParticleLayout, Access::ReadWrite>(b);
        vb.field<Position>()[0] = vec3{7.0f, 8.0f, 9.0f};
        vb.field<Mass>()[0] = 42.0f;
    }

    auto va = storage.view<ParticleLayout, Access::Read>(a);
    auto vb = storage.view<ParticleLayout, Access::Read>(b);

    assert(va.field<Position>()[0].x == 1.0f);
    assert(va.field<Mass>()[0] == 99.0f);
    assert(vb.field<Position>()[0].x == 7.0f);
    assert(vb.field<Mass>()[0] == 42.0f);

    std::printf("[PASS] Multiple Instances Of Same Type\n");
}

// ---------------------------------------------------------------------------
// Test 3: Destroying storage
// ---------------------------------------------------------------------------

void testDestroyStorage()
{
    DefaultMemoryManager memory;
    Storage storage(memory);

    StorageHandle handle = storage.create<ParticleLayout>(8);
    storage.destroy(handle);

    // After destruction the handle is no longer usable.
    expectThrow<std::invalid_argument>(
        [&]() { storage.logicalCount(handle); });

    // A new record may reuse the same slot with a fresh generation.
    StorageHandle reused = storage.create<ParticleLayout>(4);
    assert(reused.index == handle.index);
    assert(reused.generation != handle.generation);
    assert(storage.capacity(reused) == 4);

    std::printf("[PASS] Destroy Storage\n");
}

// ---------------------------------------------------------------------------
// Test 4: Invalid and stale handles
// ---------------------------------------------------------------------------

void testInvalidAndStaleHandles()
{
    DefaultMemoryManager memory;
    Storage storage(memory);

    // Out-of-range index.
    expectThrow<std::out_of_range>(
        [&]() { storage.logicalCount(StorageHandle{1000, 1}); });

    // Zero generation is treated as never-issued.
    expectThrow<std::invalid_argument>(
        [&]() { storage.logicalCount(StorageHandle{0, 0}); });

    StorageHandle handle = storage.create<ParticleLayout>(8);

    // Wrong generation for a live slot.
    expectThrow<std::invalid_argument>(
        [&]() { storage.logicalCount(StorageHandle{handle.index, handle.generation + 1}); });

    // Stale handle: destroy, then reuse the handle afterwards.
    storage.destroy(handle);
    expectThrow<std::invalid_argument>(
        [&]() { storage.logicalCount(handle); });

    std::printf("[PASS] Invalid And Stale Handles\n");
}

// ---------------------------------------------------------------------------
// Test 5: Capacity validation
// ---------------------------------------------------------------------------

void testCapacityValidation()
{
    DefaultMemoryManager memory;
    Storage storage(memory);

    // Zero capacity is rejected.
    expectThrow<std::invalid_argument>(
        [&]() { storage.create<ParticleLayout>(0); });

    // No partial record is left behind after a rejected creation.
    StorageHandle handle = storage.create<ParticleLayout>(4);
    assert(handle.index == 0);

    std::printf("[PASS] Capacity Validation\n");
}

// ---------------------------------------------------------------------------
// Test 6: Changing logical count
// ---------------------------------------------------------------------------

void testLogicalCount()
{
    DefaultMemoryManager memory;
    Storage storage(memory);

    StorageHandle handle = storage.create<ParticleLayout>(16);
    assert(storage.logicalCount(handle) == 0);

    storage.setLogicalCount(handle, 8);
    assert(storage.logicalCount(handle) == 8);
    assert(storage.capacity(handle) == 16);

    storage.setLogicalCount(handle, 0);
    assert(storage.logicalCount(handle) == 0);

    storage.setLogicalCount(handle, 16);
    assert(storage.logicalCount(handle) == 16);

    // Exceeding capacity is rejected and leaves the count unchanged.
    expectThrow<std::out_of_range>(
        [&]() { storage.setLogicalCount(handle, 17); });
    assert(storage.logicalCount(handle) == 16);

    std::printf("[PASS] Logical Count\n");
}
// ---------------------------------------------------------------------------
// Test 7: SoA field access
// ---------------------------------------------------------------------------

void testSoAFieldAccess()
{
    DefaultMemoryManager memory;
    Storage storage(memory);

    StorageHandle handle = storage.create<ParticleLayout>(8);

    {
        auto view = storage.view<ParticleLayout, Access::ReadWrite>(handle);
        view.field<Position>()[0] = vec3{1.0f, 2.0f, 3.0f};
        view.field<Position>()[1] = vec3{4.0f, 5.0f, 6.0f};
        view.field<Velocity>()[0] = vec3{9.0f, 8.0f, 7.0f};
        view.field<Mass>()[0] = 0.5f;
        view.field<Mass>()[7] = 2.5f;
    }

    auto view = storage.view<ParticleLayout, Access::Read>(handle);

    assert(view.field<Position>()[0].x == 1.0f);
    assert(view.field<Position>()[1].z == 6.0f);
    assert(view.field<Velocity>()[0].y == 8.0f);
    assert(view.field<Mass>()[0] == 0.5f);
    assert(view.field<Mass>()[7] == 2.5f);

    std::printf("[PASS] SoA Field Access\n");
}

// ---------------------------------------------------------------------------
// Test 8: Field isolation
// ---------------------------------------------------------------------------

void testFieldIsolation()
{
    DefaultMemoryManager memory;
    Storage storage(memory);

    StorageHandle handle = storage.create<ParticleLayout>(4);

    {
        auto view = storage.view<ParticleLayout, Access::ReadWrite>(handle);

        // Fill each field with a distinct non-overlapping pattern and make sure
        // writing one field never touches a sibling field's buffer.
        const vec3 v0 = {1.0f, 0.0f, 0.0f};
        const vec3 v1 = {0.0f, 1.0f, 0.0f};

        for (size_t i = 0; i < 4; ++i)
        {
            view.field<Position>()[i] = v0;
            view.field<Velocity>()[i] = v1;
            view.field<Mass>()[i] = static_cast<f32>(i);
        }

        // Rewrite the velocity field; positions and masses must be untouched.
        for (size_t i = 0; i < 4; ++i)
        {
            view.field<Velocity>()[i] = vec3{2.0f, 2.0f, 2.0f};
        }
    }

    auto view = storage.view<ParticleLayout, Access::Read>(handle);

    for (size_t i = 0; i < 4; ++i)
    {
        assert(view.field<Position>()[i].x == 1.0f);
        assert(view.field<Position>()[i].y == 0.0f);
        assert(view.field<Velocity>()[i].x == 2.0f);
        assert(view.field<Mass>()[i] == static_cast<f32>(i));
    }

    std::printf("[PASS] Field Isolation\n");
}

// ---------------------------------------------------------------------------
// Test 9: Read and Write views
// ---------------------------------------------------------------------------

void testReadWriteViews()
{
    DefaultMemoryManager memory;
    Storage storage(memory);

    StorageHandle handle = storage.create<ParticleLayout>(8);

    // Write view exposes mutable storage.
    {
        auto view = storage.view<ParticleLayout, Access::ReadWrite>(handle);

        view.field<Position>()[0] = vec3{3.0f, 3.0f, 3.0f};
        view.field<Velocity>()[0] = vec3{1.0f, 0.0f, 0.0f};
        view.field<Mass>()[0] = 10.0f;
    }

    // ReadWrite view also exposes mutable storage.
    {
        auto rw = storage.view<ParticleLayout, Access::ReadWrite>(handle);

        rw.field<Position>()[1] = vec3{4.0f, 4.0f, 4.0f};
        rw.field<Velocity>()[1] = vec3{0.0f, 1.0f, 0.0f};
        rw.field<Mass>()[1] = 20.0f;
    }

    // Read view exposes immutable storage.
    {
        auto view = storage.view<ParticleLayout, Access::Read>(handle);

        assert(view.field<Position>()[0].y == 3.0f);
        assert(view.field<Position>()[1].y == 4.0f);

        assert(view.field<Velocity>()[0].x == 1.0f);
        assert(view.field<Velocity>()[1].y == 1.0f);

        assert(view.field<Mass>()[0] == 10.0f);
        assert(view.field<Mass>()[1] == 20.0f);
    }

    std::printf("[PASS] Read And Write Views\n");
}

// ---------------------------------------------------------------------------
// Test 10: Rejecting simultaneous views
// ---------------------------------------------------------------------------

void testRejectSimultaneousViews()
{
    DefaultMemoryManager memory;
    Storage storage(memory);

    StorageHandle handle = storage.create<ParticleLayout>(8);

    {
        auto first = storage.view<ParticleLayout, Access::ReadWrite>(handle);

        expectThrow<std::runtime_error>(
            [&]() { storage.view<ParticleLayout, Access::Read>(handle); });
        expectThrow<std::runtime_error>(
            [&]() { storage.view<ParticleLayout, Access::ReadWrite>(handle); });
        expectThrow<std::runtime_error>(
            [&]() { storage.view<ParticleLayout, Access::ReadWrite>(handle); });
    }

    auto second = storage.view<ParticleLayout, Access::Read>(handle);
    assert(second.count() == 0);

    std::printf("[PASS] Reject Simultaneous Views\n");
}

// ---------------------------------------------------------------------------
// Test 11: Rejecting destruction while a View is active
// ---------------------------------------------------------------------------

void testRejectDestroyWithActiveView()
{
    DefaultMemoryManager memory;
    Storage storage(memory);

    StorageHandle handle = storage.create<ParticleLayout>(8);

    {
        auto view = storage.view<ParticleLayout, Access::ReadWrite>(handle);

        expectThrow<std::runtime_error>(
            [&]() { storage.destroy(handle); });
    }

    // After the View is gone, destruction succeeds.
    storage.destroy(handle);

    std::printf("[PASS] Reject Destroy With Active View\n");
}

// ---------------------------------------------------------------------------
// Test 12: Allocation failure and rollback
// ---------------------------------------------------------------------------

void testAllocationRollback()
{
    DefaultMemoryManager base;

    // Fail on the second field buffer of ThreeU64Layout (3 fields).
    FailingMemoryManager failing(base, 2);
    Storage storage(failing);

    expectThrow<std::bad_alloc>(
        [&]() { storage.create<ThreeU64Layout>(8); });

    // Exactly the first (successful) buffer must have been released again.
    assert(failing.createCalls() == 2);
    assert(failing.destroyCalls() == 1);

    // No partial record was registered: the next valid creation gets slot 0.
    StorageHandle handle = storage.create<ThreeU64Layout>(8);
    assert(handle.index == 0);

    std::printf("[PASS] Allocation Failure And Rollback\n");
}

// ---------------------------------------------------------------------------
// Test 13: Memory alignment
// ---------------------------------------------------------------------------

void testMemoryAlignment()
{
    DefaultMemoryManager memory;
    Storage storage(memory);

    StorageHandle handle = storage.create<AlignLayout>(4);

    auto view = storage.view<AlignLayout, Access::Read>(handle);

    const Aligned64* ptr = view.field<0>();
    assert(isAligned(ptr, 64));

    std::printf("[PASS] Memory Alignment\n");
}

// ---------------------------------------------------------------------------
// Mismatched layout handle
// ---------------------------------------------------------------------------

void testMismatchedLayoutHandle()
{
    DefaultMemoryManager memory;
    Storage storage(memory);

    StorageHandle particle = storage.create<ParticleLayout>(4);

    // Requesting a view over a record created with a different layout must be
    // rejected even though the handle itself is valid.
    expectThrow<std::invalid_argument>(
        [&]() { storage.view<ThreeU64Layout, Access::Read>(particle); });

    std::printf("[PASS] Mismatched Layout Handle\n");
}

// 
// struct Shape //non trivially copyable
// {
//     float area = 0.0f;
//     virtual ~Shape() = default;
// };

// void testTriviallyCopyable(){
//     [[maybe_unused]] Field<Shape> bad1;
// }