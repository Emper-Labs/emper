#pragma once

#include <memory>

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

};


}