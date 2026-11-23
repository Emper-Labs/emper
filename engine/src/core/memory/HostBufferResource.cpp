
#include <new>
#include <emper/core/memory/HostBufferResource.hpp>

namespace emper
{

HostBufferResource::HostBufferResource(IAllocator& allocator, size_t size, size_t alignment)
    : m_allocator(&allocator), m_size(size), m_alignment(alignment)
{
    m_ptr = m_allocator->allocate(m_size, m_alignment);
    if (!m_ptr) {
        throw std::bad_alloc{};
    }
}

HostBufferResource::~HostBufferResource() noexcept
{
    if (m_ptr && m_allocator) {
        m_allocator->deallocate(m_ptr, m_alignment);
    }
}

size_t HostBufferResource::size() const noexcept
{
    return m_size;
}

void* HostBufferResource::data() noexcept
{
    return m_ptr;
}

const void* HostBufferResource::data() const noexcept
{
    return m_ptr;
}

void* HostBufferResource::map()
{
    return m_ptr;
}

void HostBufferResource::unmap()
{
    // no need
}

}   