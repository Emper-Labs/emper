#pragma once

#include <emper/core/memory/IAllocator.hpp>
#include <emper/core/memory/IBufferResource.hpp> 

namespace emper
{

class HostBufferResource final : public IBufferResource
{
public:

    //using unique_ptr not just cp
    HostBufferResource(const HostBufferResource&) = delete; 
    HostBufferResource& operator=(const HostBufferResource&) = delete;

    HostBufferResource(IAllocator& allocator, size_t size, size_t alignment);
    ~HostBufferResource() noexcept override;

    size_t size() const noexcept override;
    void* data() noexcept;
    const void* data() const noexcept;
    void *map() override;
    void unmap() override;

private:
    IAllocator* m_allocator = nullptr; // this ptr will allway able
    void* m_ptr = nullptr;
    size_t m_size = 0;
    size_t m_alignment = 0;
};

}