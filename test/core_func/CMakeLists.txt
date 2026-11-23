add_executable(core_func_test
    main.cpp
    LifeCycle.cpp
    HostBufferResourceTests.cpp
    HeapAllocatorTests.cpp
)

target_link_libraries(core_func_test PRIVATE emper::engine)