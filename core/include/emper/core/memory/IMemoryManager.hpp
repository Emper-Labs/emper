#pragma once

#include <emper/core/memory/Buffer.hpp>

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
}
