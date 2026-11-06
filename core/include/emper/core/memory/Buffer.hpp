#pragma once 

#include <emper/type.hpp>


namespace emper
{

enum class MemoryDomain : u8
{
Host = 0,
Device,
Shared
};

enum class BufferUsage : u32
{
    None     = 0,
    Storage  = 1u << 0,
    Uniform  = 1u << 1,
    Vertex   = 1u << 2,
    Index    = 1u << 3,
    Transfer = 1u << 4    
};

constexpr BufferUsage operator|(
        BufferUsage lhs,
        BufferUsage rhs)
{
    return static_cast<BufferUsage>(
        static_cast<u32>(lhs) | static_cast<u32>(rhs)
    );
}

struct BufferDesc
{
    size_t size;
    size_t alignment;
    MemoryDomain domain;
    BufferUsage usage;
    
    constexpr bool hasUsage(
        BufferUsage usage, 
        BufferUsage flag) noexcept
    {
       return (static_cast<u32>(usage) & static_cast<u32>(flag))
        == static_cast<u32>(flag);
    }
};

struct BufferHandle
{
    u64 index;
    u64 generation;
};

class Buffer
{
public:
    Buffer(BufferHandle handle) : m_handle(handle) {};
    
    bool valid() const noexcept{
        return m_handle.generation != 0;
    }


    const BufferHandle& handle() const noexcept
    {
        return m_handle;
    }

private:
    BufferHandle m_handle = {0,0};
};


class IBufferResource
{
public:
    virtual ~IBufferResource() = default;

    virtual size_t size() const = 0;
};

/*

Simulation / World
       |
       v
     Buffer
       |
       v
     Resource
       |
       v
   Allocation
       |
       v
    Allocator


World
 └── Buffer
       └── Resource Handle

MemoryManager
 └── Allocator
       └── Allocation

Backend
 └── Resource implementation



*/


}