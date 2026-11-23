#pragma once

#include <emper/type.hpp>

namespace emper{

class IBufferResource
{
public:
    virtual ~IBufferResource() = default;

    virtual size_t size() const = 0;

    virtual void* map() = 0;
    virtual void unmap() = 0;
};

}