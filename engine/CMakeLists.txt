add_library(emper_engine
    src/simulation/Simulation.cpp
    src/simulation/SystemScheduler.cpp

    src/core/memory/HostBufferResource.cpp
    src/core/memory/HeapAllocator.cpp  
    src/core/memory/DefaultMemoryManager.cpp
    src/core/memory/Storage.cpp
)
add_library(emper::engine ALIAS emper_engine)

target_include_directories(emper_engine PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_compile_features(emper_engine PUBLIC cxx_std_20)