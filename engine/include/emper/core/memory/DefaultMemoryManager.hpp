#pragma once 


#include <memory>
#include <vector>
#include <emper/core/memory/IAllocator.hpp>
#include <emper/core/memory/IMemoryManager.hpp>
#include <emper/core/memory/IBufferResource.hpp>

namespace emper{

struct BufferRecord{
    bool valid = false;
    bool mapped = false;

    BufferDesc desc;
    BufferHandle handle;
    
    std::unique_ptr<IBufferResource> resource;
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