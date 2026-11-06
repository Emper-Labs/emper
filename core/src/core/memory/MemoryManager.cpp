#include <emper/core/memory/MemoryManager.hpp>
#include <emper/core/memory/HeapAllocator.hpp>

#include <limits>

namespace emper
{

DefaultMemoryManager::DefaultMemoryManager()
    : m_hostAllocator(std::make_unique<HeapAllocator>())
{

    //later
    // if(!m_hostAllocator || !m_deviceAllocator || !m_sharedAllocator)
    // {
    //     throw std::invalid_argument("Allocators cannot be null");
    // }
}

DefaultMemoryManager::~DefaultMemoryManager()
{
    for (auto& record : m_bufferRecords)
    {
        if (record.valid)
        {
            releaseBuffer(record);
        }
    }
}

emper::Buffer DefaultMemoryManager::createBuffer(
    const BufferDesc& desc)
{
    // 0. validate desc
    validateBufferDesc(desc);

    // 1. select allocator

    IAllocator* allocator = nullptr;

    switch (desc.domain)
    {
        case MemoryDomain::Host:
            allocator = m_hostAllocator.get();
            break;

        // case MemoryDomain::Device:
        //     allocator = m_deviceAllocator.get();
        //     break;

        // case MemoryDomain::Shared:
        //     allocator = m_sharedAllocator.get();
        //     break;

        default:
            throw std::runtime_error("Invalid memory domain");
    }

    if (!allocator)
    {
        throw std::runtime_error("Allocator is not available");
    }

    // 2. find reusable slot
    BufferRecord* reusableRecord = nullptr;
    BufferHandle handle{};

    for (u64 i = 0; i < m_bufferRecords.size(); ++i)
    {
        auto& record = m_bufferRecords[i];

        if (!record.valid &&
            record.handle.generation !=
                std::numeric_limits<u64>::max())
        {
            reusableRecord = &record;

            handle.index = i;
            handle.generation = record.handle.generation + 1;

            break;
        }
    }

    // 3. create a new slot if necessary
    if (!reusableRecord)
    {
        handle.index = static_cast<u64>(m_bufferRecords.size());
        handle.generation = 1;
    }

    // 4. alloc
    void* ptr = allocator->allocate(desc.size, desc.alignment);

    if (!ptr)
    {
        throw std::bad_alloc();
    }

    // reg record
    if (reusableRecord)
    {
        reusableRecord->valid = true;
        reusableRecord->ptr = ptr;
        reusableRecord->desc = desc;
        reusableRecord->handle = handle;
    }
    else
    {
        try
        {
            m_bufferRecords.push_back({
                true,
                false,
                ptr,
                desc,
                handle
            });
        }
        catch (...)
        {
            allocator->deallocate(ptr, desc.alignment);
            throw;
        }
    }

    return Buffer(handle);
}

void DefaultMemoryManager::destroyBuffer(
    const Buffer& buffer)
{
    if (!buffer.valid())
    {
        throw std::invalid_argument("Invalid buffer handle");
    }

    const BufferHandle& handle = buffer.handle();
    if (handle.index >= m_bufferRecords.size())
    {
        throw std::out_of_range("Buffer index out of range");
    }

    BufferRecord& record = m_bufferRecords[handle.index];
    if (!record.valid || record.handle.generation != handle.generation)
    {
        throw std::invalid_argument("Buffer has already been destroyed or is invalid");
    }

    if(record.mapped)
    {
        throw std::runtime_error("Cannot destroy a mapped buffer");
    }

    releaseBuffer(record);
}

void DefaultMemoryManager::validateBufferDesc(
    const BufferDesc& desc) const
{
    if (desc.size == 0)
    {
        throw std::invalid_argument(
            "Buffer size cannot be zero"
        );
    }

    if (desc.alignment == 0 ||
        (desc.alignment & (desc.alignment - 1)) != 0)
    {
        throw std::invalid_argument(
            "Buffer alignment must be a power of two"
        );
    }

    if (desc.domain != MemoryDomain::Host)
    {
        throw std::invalid_argument(
            "Unsupported memory domain"
        );
    }
}

void DefaultMemoryManager::releaseBuffer(
    BufferRecord& record) noexcept
{
    if (!record.valid)
        return;

    if (record.mapped)
        std::terminate();

    if (!record.ptr)
        std::terminate();

    IAllocator* allocator = nullptr;

    switch (record.desc.domain)
    {
        case MemoryDomain::Host:
            allocator = m_hostAllocator.get();
            break;

        default:
            std::terminate();
    }

    if (!allocator)
        std::terminate();

    allocator->deallocate(
        record.ptr,
        record.desc.alignment
    );

    record.ptr = nullptr;
    record.mapped = false;
    record.valid = false;
}

void DefaultMemoryManager::validateHostAccess(
    const BufferRecord& record) const
{
    if (record.desc.domain != MemoryDomain::Host)
    {
        throw std::runtime_error(
            "Buffer is not host accessible"
        );
    }

    if (!record.ptr)
    {
        throw std::runtime_error(
            "Buffer has no host memory allocation"
        );
    }
}

void* DefaultMemoryManager::mapBuffer(const Buffer& buffer)
{
    if (!buffer.valid())
    {
        throw std::invalid_argument("Invalid buffer handle");
    }

    const BufferHandle& handle = buffer.handle();
    if (handle.index >= m_bufferRecords.size())
    {
        throw std::out_of_range("Buffer index out of range");
    }

    BufferRecord& record = m_bufferRecords[handle.index];
    if (!record.valid || record.handle.generation != handle.generation)
    {
        throw std::invalid_argument("Buffer has already been destroyed or is invalid");
    }

    validateHostAccess(record);

    if (record.mapped)
    {
        throw std::runtime_error("Buffer is already mapped");
    }

    record.mapped = true;
    return record.ptr;
}


void DefaultMemoryManager::unmapBuffer(const Buffer& buffer)
{
    if (!buffer.valid())
    {
        throw std::invalid_argument("Invalid buffer handle");
    }

    const BufferHandle& handle = buffer.handle();
    if (handle.index >= m_bufferRecords.size())
    {
        throw std::out_of_range("Buffer index out of range");
    }

    BufferRecord& record = m_bufferRecords[handle.index];
    if (!record.valid || record.handle.generation != handle.generation)
    {
        throw std::invalid_argument("Buffer has already been destroyed or is invalid");
    }

    if (!record.mapped)
    {
        throw std::runtime_error("Buffer is not mapped");
    }

    record.mapped = false;
}


}