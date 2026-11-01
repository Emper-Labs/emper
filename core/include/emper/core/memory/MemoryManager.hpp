#pragma once

#include <memory>
#include <vector>

#include <emper/core/memory/Buffer.hpp>
#include <emper/core/memory/IAllocator.hpp>



namespace emper{

class IMemoryManager
{
public:
    IMemoryManager() = default;
    virtual ~IMemoryManager() = default;

    virtual Buffer createBuffer(
        const BufferDesc& desc) = 0;

    virtual void destroyBuffer(
        const Buffer& buffer) = 0;

};

struct BufferRecord{
    bool valid = false;

    BufferDesc desc;
    BufferHandle handle;
};

class DefaultMemoryManager final : public IMemoryManager
{
public:
    DefaultMemoryManager(
        std::unique_ptr<IAllocator> hostAllocator,
        std::unique_ptr<IAllocator> deviceAllocator,
        std::unique_ptr<IAllocator> sharedAllocator);

    Buffer createBuffer(
        const BufferDesc& desc) override;

    void destroyBuffer(
        const Buffer& buffer) override;

private:    
    std::unique_ptr<IAllocator> m_hostAllocator;
    std::unique_ptr<IAllocator> m_deviceAllocator;
    std::unique_ptr<IAllocator> m_sharedAllocator;

    std::vector<BufferRecord> m_bufferRecords;//dont use erase, just mark as invalid
};


}