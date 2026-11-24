#include <stdexcept>

#include <emper/core/memory/Storage.hpp>

namespace emper
{

Storage::Storage(IMemoryManager& memoryManager)
    : m_memoryManager(memoryManager)
{
}

Storage::~Storage()
{
    for (auto& record : m_records)
    {
        if (record.valid)
        {
            releaseRecord(record);
        }
    }
}

//this fn needs to be arranged in the correct order, like this haha
StorageRecord& Storage::recordFor(const StorageHandle& handle)
{
    if (handle.generation == 0)
        throw std::invalid_argument("Invalid storage handle generation");

    if (handle.index >= m_records.size())
        throw std::out_of_range("Storage index out of range");

    StorageRecord& record = m_records[handle.index];

    if (!record.valid || record.generation != handle.generation)
        throw std::invalid_argument("Invalid or stale storage handle");

    return record;
}

const StorageRecord& Storage::recordFor(const StorageHandle& handle) const
{
    if (handle.generation == 0)
        throw std::invalid_argument("Invalid storage handle generation");

    if (handle.index >= m_records.size())
        throw std::out_of_range("Storage index out of range");

    const StorageRecord& record = m_records[handle.index];

    if (!record.valid || record.generation != handle.generation)
        throw std::invalid_argument("Invalid or stale storage handle");

    return record;
}
//end


void Storage::destroy(const StorageHandle& handle)
{
    StorageRecord& record = recordFor(handle);

    if (record.viewActive)
    {
        throw std::runtime_error(
            "Cannot destroy storage while a View is active");
    }

    releaseRecord(record);
}

void Storage::setLogicalCount(const StorageHandle& handle, size_t count)
{
    StorageRecord& record = recordFor(handle);

    if (count > record.capacity)
    {
        throw std::out_of_range(
            "Logical count cannot exceed capacity");
    }

    record.count = count;
}

u64 Storage::logicalCount(const StorageHandle& handle) const
{
    return recordFor(handle).count;
}

u64 Storage::capacity(const StorageHandle& handle) const
{
    return recordFor(handle).capacity;
}

IMemoryManager& Storage::memoryManager() const noexcept
{
    return m_memoryManager;
}

void Storage::releaseView(const StorageHandle& handle)
{
    StorageRecord& record = recordFor(handle);

    if (!record.viewActive)
    {
        return;
    }

    for (auto& field : record.fields)
    {
        m_memoryManager.unmapBuffer(field.buffer);
    }

    record.viewActive = false;
}

void Storage::releaseRecord(StorageRecord& record) noexcept
{
    if (!record.valid)
    {
        return;
    }

    for (auto& field : record.fields)
    {
        try
        {
            m_memoryManager.destroyBuffer(field.buffer);
        }
        catch (...)
        {
            // Best effort during teardown; the record is torn down regardless.
        }
    }

    record.fields.clear();
    record.valid = false;
    record.viewActive = false;
    record.count = 0;
    record.capacity = 0;
    record.layoutInfo = nullptr;
    // `generation` is intentionally preserved so the slot can be reused with an
    // incremented generation, invalidating stale handles to this slot.
}

} // namespace emper