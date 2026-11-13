add_library(emper_core
    src/core/memory/HostBufferResource.cpp
    src/core/memory/HeapAllocator.cpp
    src/simulation/Simulation.cpp
    src/core/memory/MemoryManager.cpp
)
add_library(emper::core ALIAS emper_core)

target_include_directories(emper_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_compile_features(emper_core PUBLIC cxx_std_20)