#include <emper/type.hpp>

namespace emper{

struct StorageRecord
{
    bool valid = false;

    u64 generation = 0;

    //TypeId type;

    u64 count = 0;
    u64 capacity = 0;

    //std::vector<Buffer> buffers;
};

class Storage{

};


}