add_library(emper_core
    src/core/HeapAllocator.cpp
)
add_library(emper::core ALIAS emper_core)

target_include_directories(emper_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_compile_features(emper_core PUBLIC cxx_std_20)