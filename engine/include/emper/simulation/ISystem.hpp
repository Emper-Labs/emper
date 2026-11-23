#pragma once

#include <emper/type.hpp>

namespace emper{


class Storage; 
struct SystemContext
{
    Storage* storage = nullptr;
};

class ISystem
{
public:
    virtual ~ISystem() = default;

    virtual bool initialize(SystemContext& context) = 0;
    
    virtual void update(
        SystemContext& context,
        f64 dt
    ) = 0;

    virtual void shutdown(SystemContext& context) = 0;
};

}