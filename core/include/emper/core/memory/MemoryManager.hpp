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


    //host group
    
    virtual void* mapBuffer(
        const Buffer& buffer) = 0;

    virtual void unmapBuffer(
        const Buffer& buffer) = 0;
    
    //


};

struct BufferRecord{
    bool valid = false;
    bool mapped = false;

    void* ptr = nullptr;
    

    BufferDesc desc;
    BufferHandle handle;
};

class DefaultMemoryManager final : public IMemoryManager
{
public:
    ~DefaultMemoryManager() noexcept override ;

    DefaultMemoryManager(
    );

    Buffer createBuffer(
        const BufferDesc& desc) override;

    void destroyBuffer(
        const Buffer& buffer) override;
    
    void* mapBuffer(
        const Buffer& buffer) override;

    void unmapBuffer(
        const Buffer& buffer) override;

private:    

    void validateBufferDesc(const BufferDesc& desc) const;
    void validateHostAccess(const BufferRecord& record) const;
    
    void releaseBuffer(
        BufferRecord& record) noexcept;


    std::unique_ptr<IAllocator> m_hostAllocator;
    // std::unique_ptr<IAllocator> m_deviceAllocator;
    // std::unique_ptr<IAllocator> m_sharedAllocator;

    std::vector<BufferRecord> m_bufferRecords;//dont use erase, just mark as invalid
};


}