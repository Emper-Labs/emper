#pragma once

#include <cstdint>
#include <stdexcept>
#include <typeinfo>
#include <utility>
#include <vector>
#include <type_traits>

#include <emper/type.hpp>
#include <emper/core/memory/Buffer.hpp>
#include <emper/core/memory/IMemoryManager.hpp>

namespace emper
{

// ---------------------------------------------------------------------------
// Access modes for Storage views.
// ---------------------------------------------------------------------------

enum class Access : u8
{
    Read      = 0,
    ReadWrite = 1
};

// ---------------------------------------------------------------------------
// Storage handle. Mirrors BufferHandle: index + generation. A stale / reused
// slot invalidates old handles because the generation is stamped on reuse.
// ---------------------------------------------------------------------------

struct StorageHandle
{
    u64 index;
    u64 generation;
};


template <typename T, typename Tag = void>
struct Field
{
    using Type = T;
    using TagType = Tag;

    static constexpr size_t alignment = alignof(T);
};

template <typename... Fields>
struct SoALayout
{
    static constexpr u64 fieldCount = sizeof...(Fields);
};

namespace detail
{
template <typename T, typename... Fields>
inline constexpr u64 FieldTagCount =
    (u64{0} + ... +
     (std::is_same_v<T, typename Fields::TagType> ? u64{1} : u64{0}));

template <typename Tag, typename... Fields>
consteval u64 FindFieldIndex()
{
    if constexpr (sizeof...(Fields) == 0)
    {
        return 0;
    }
    else
    {
        constexpr bool matches[] = {
            std::is_same_v<Tag, typename Fields::TagType>...
        };

        for (u64 i = 0; i < sizeof...(Fields); ++i)
        {
            if (matches[i])
                return i;
        }

        return 0;
    }
}

template <u64 I, typename... Fields>
struct FieldAt
{
};

template <u64 I, typename Head, typename... Tail>
struct FieldAt<I, Head, Tail...> : FieldAt<I - 1, Tail...>
{
};

template <typename Head, typename... Tail>
struct FieldAt<0, Head, Tail...>
{
    using Type = typename Head::Type;
};

template <u64 I, typename Layout>
struct LayoutFieldAt;

template <u64 I, typename... Fields>
struct LayoutFieldAt<I, SoALayout<Fields...>>
{
    static_assert(I < sizeof...(Fields),
                  "Field index out of range");

    using Type = typename FieldAt<I, Fields...>::Type;
};

template <typename Tag, typename Layout>
struct LayoutFieldIndex;

template <typename Tag, typename... Fields>
struct LayoutFieldIndex<Tag, SoALayout<Fields...>>
{
    static_assert(
        FieldTagCount<Tag, Fields...> == 1,
        "Field tag must appear exactly once in layout");

    static constexpr u64 value = FindFieldIndex<Tag, Fields...>();
};
} // namespace detail

// ---------------------------------------------------------------------------
// Forward declarations.
// ---------------------------------------------------------------------------

class Storage;

template <typename Layout, Access Mode>
class View;

// ---------------------------------------------------------------------------
// Storage
// ---------------------------------------------------------------------------

struct FieldBufferInfo
{
    Buffer buffer;
    size_t elementSize = 0;
    size_t alignment = 0;
};

struct StorageRecord
{
    bool valid = false;
    bool viewActive = false;

    u64 generation = 0;
    u64 count = 0;
    u64 capacity = 0;

    // Identity of the SoA layout used to create this record.
    const std::type_info* layoutInfo = nullptr;

    // One aligned buffer per SoA field.
    std::vector<FieldBufferInfo> fields;
};

class Storage
{
public:
    // A View must be able to acquire/release the exclusive record view.
    template <typename Layout, Access Mode>
    friend class View;

    Storage(const Storage&) = delete;
    Storage& operator=(const Storage&) = delete;

    explicit Storage(IMemoryManager& memoryManager);

    ~Storage();

    // Allocate a new typed record with the given SoA layout and fixed capacity.
    // Transactional: if any field buffer cannot be allocated, previously
    // allocated buffers are released and an exception is thrown.
    template <typename Layout>
    StorageHandle create(size_t capacity);

    // Destroy a record. Rejected while a View is active.
    void destroy(const StorageHandle& handle);

    // Change the logical element count within [0, capacity]. Preserves existing
    // elements; does not initialize newly activated elements.
    void setLogicalCount(const StorageHandle& handle, size_t count);

    u64 logicalCount(const StorageHandle& handle) const;
    u64 capacity(const StorageHandle& handle) const;

    template <typename Layout, Access Mode>
    View<Layout, Mode> view(const StorageHandle& handle);

    IMemoryManager& memoryManager() const noexcept;

private:
    StorageRecord& recordFor(const StorageHandle& handle);
    const StorageRecord& recordFor(const StorageHandle& handle) const;

    template <typename Layout>
    std::vector<void*> acquireView(const StorageHandle& handle);
    void releaseView(const StorageHandle& handle);

    template <typename T>
    Buffer makeFieldBuffer(size_t capacity);

    template <typename Layout>
    void createFieldBuffers(size_t capacity, std::vector<Buffer>& out);

    void releaseRecord(StorageRecord& record) noexcept;

    IMemoryManager& m_memoryManager;
    std::vector<StorageRecord> m_records;
};

// ---------------------------------------------------------------------------
// RAII typed View.
//
// A View holds an active, exclusive view over one storage record. Construction
// validates the handle and layout, maps all field buffers and marks the record
// as busy (only one View per record may be active). Destruction unmaps the
// buffers and releases the record.
//
// `Access` is a template parameter so const-correctness is enforced by the
// compiler: a Read View returns `const T*` field pointers; a Write or ReadWrite
// View returns mutable `T*`.
// ---------------------------------------------------------------------------

template <typename Layout, Access Mode>
class View
{
public:
    View(const View&) = delete;
    View& operator=(const View&) = delete;

    View(View&& other) noexcept
        : m_storage(other.m_storage),
          m_handle(other.m_handle),
          m_fields(std::move(other.m_fields)),
          m_count(other.m_count)
    {
        other.m_storage = nullptr;
        other.m_fields.clear();
        other.m_count = 0;
    }

    View& operator=(View&&) = delete;

    ~View()
    {
        if (m_storage)
        {
            m_storage->releaseView(m_handle);
        }
    }

    // Typed access to field array `I`.
    template <u64 I>
    auto field() const noexcept
    {
        static_assert(I < Layout::fieldCount, "Field index out of range");

        using T = typename detail::LayoutFieldAt<I, Layout>::Type;

        if constexpr (Mode == Access::Read)
        {
            return static_cast<const T*>(m_fields[I]);
        }
        else
        {
            return static_cast<T*>(m_fields[I]);
        }
    }

    template <typename Tag>
    auto field() const noexcept
    {   
        constexpr u64 index =
            detail::LayoutFieldIndex<Tag, Layout>::value;

        using T = typename detail::LayoutFieldAt<index, Layout>::Type;

        if constexpr (Mode == Access::Read)
            return static_cast<const T*>(m_fields[index]);
        else
            return static_cast<T*>(m_fields[index]);
    }

    u64 count() const noexcept
    {
        return m_count;
    }

    StorageHandle handle() const noexcept
    {
        return m_handle;
    }

private:
    friend class Storage;

    View(Storage& storage, const StorageHandle& handle)
        : m_storage(&storage),
          m_handle(handle)
    {
        m_fields = m_storage->acquireView<Layout>(handle);
        m_count = m_storage->logicalCount(handle);
        m_fields.resize(Layout::fieldCount);
    }

    Storage* m_storage = nullptr;
    StorageHandle m_handle = {0, 0};
    std::vector<void*> m_fields;
    u64 m_count = 0;
};

template <typename T>
Buffer Storage::makeFieldBuffer(size_t capacity)
{
    BufferDesc desc;
    desc.size = capacity * sizeof(T);
    desc.alignment = alignof(T);
    desc.domain = MemoryDomain::Host;
    desc.usage = BufferUsage::Storage;

    return m_memoryManager.createBuffer(desc);
}

template <typename Layout>
void Storage::createFieldBuffers(size_t capacity, std::vector<Buffer>& out)
{
    constexpr u64 count = Layout::fieldCount;

    [&]<u64... Is>(std::index_sequence<Is...>)
    {
        ((out.push_back(
            makeFieldBuffer<
                typename detail::LayoutFieldAt<Is, Layout>::Type>(
                capacity)), ...), 0);
    }(std::make_index_sequence<count>{});
}

template <typename Layout>
StorageHandle Storage::create(size_t capacity)
{
    if (capacity == 0)
    {
        throw std::invalid_argument(
            "Storage capacity cannot be zero");
    }

    // Phase 1: allocate every field buffer transactionally.
    std::vector<Buffer> fieldBuffers;
    fieldBuffers.reserve(Layout::fieldCount);

    try
    {
        createFieldBuffers<Layout>(capacity, fieldBuffers);
    }
    catch (...)
    {
        for (auto& buffer : fieldBuffers)
        {
            m_memoryManager.destroyBuffer(buffer);
        }
        throw;
    }

    // Phase 2: find a reusable slot or append a new one (generation-stamped).
    StorageRecord* reusableRecord = nullptr;
    StorageHandle handle{};

    for (u64 i = 0; i < m_records.size(); ++i)
    {
        auto& record = m_records[i];

        if (!record.valid && record.generation != 0)
        {
            reusableRecord = &record;
            handle.index = i;

            if(std::numeric_limits<u64>::max() ==  record.generation)
            {
                reusableRecord = nullptr;
            }
            else handle.generation = record.generation + 1;
            
            break;
        }
    }

    if (!reusableRecord)
    {
        handle.index = static_cast<u64>(m_records.size());
        handle.generation = 1;
    }

    // Phase 3: build the runtime record.

    try{
        std::vector<FieldBufferInfo> fieldInfos;
        fieldInfos.reserve(Layout::fieldCount);
    


    [&]<u64... Is>(std::index_sequence<Is...>)
    {
        ((fieldInfos.push_back(FieldBufferInfo{
             fieldBuffers[Is],
             sizeof(typename detail::LayoutFieldAt<Is, Layout>::Type),
             alignof(typename detail::LayoutFieldAt<Is, Layout>::Type),
         }), ...), 0);
    }(std::make_index_sequence<Layout::fieldCount>{});


    
    if (reusableRecord)
    {
        *reusableRecord = StorageRecord{
            true,
            false,
            handle.generation,
            0,
            capacity,
            &typeid(Layout),
            std::move(fieldInfos),
        };
    }
    else
    {
        m_records.push_back(StorageRecord{
            true,
            false,
            handle.generation,
            0,
            capacity,
            &typeid(Layout),
            std::move(fieldInfos),
        });
    }
    }    
    catch (...)
    {
        for (auto& buffer : fieldBuffers)
        {
            m_memoryManager.destroyBuffer(buffer);
        }
        throw;
    }

    return handle;
}

template <typename Layout, Access Mode>
View<Layout, Mode> Storage::view(const StorageHandle& handle)
{
    return View<Layout, Mode>(*this, handle);
}

template <typename Layout>
std::vector<void*> Storage::acquireView(const StorageHandle& handle)
{
    StorageRecord& record = recordFor(handle);

    if (record.layoutInfo != &typeid(Layout))
    {
        throw std::invalid_argument(
            "Storage handle layout does not match the requested field types");
    }

    if (record.viewActive)
    {
        throw std::runtime_error(
            "Storage record already has an active View");
    }

    std::vector<void*> pointers;
    pointers.reserve(record.fields.size());

    size_t mapped = 0;
    try
    {
        for (size_t i = 0; i < record.fields.size(); ++i)
        {
            pointers.push_back(
                m_memoryManager.mapBuffer(record.fields[i].buffer));
            ++mapped;
        }
    }
    catch (...)
    {
        for (size_t i = 0; i < mapped; ++i)
        {
            m_memoryManager.unmapBuffer(record.fields[i].buffer);
        }
        throw;
    }

    record.viewActive = true;
    return pointers;
}

}