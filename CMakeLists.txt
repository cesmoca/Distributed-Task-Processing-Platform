cmake_minimum_required(VERSION 3.20)

project(distributed-task-platform VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

add_library(distributed_task_platform
 "src/common/task.h"
 "src/common/task.cpp"
 "src/common/thread_safe_queue.h"
 "src/common/thread_safe_queue.cpp" 
 "src/worker/worker.h" 
 "src/worker/worker_impl.h" 
 "src/worker/worker_pool.h" 
 "src/worker/worker_pool.cpp")

target_include_directories(distributed_task_platform
    PUBLIC
       ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(distributed_task_platform PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(distributed_task_platform PRIVATE /W4 /permissive-)
else()
    target_compile_options(distributed_task_platform PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(distributed_task_platform_demo src/main.cpp)
target_link_libraries(distributed_task_platform_demo PRIVATE distributed_task_platform)

if(BUILD_TESTING)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(distributed_task_platform_tests
     "tests/common/task_test.cpp"
     "tests/common/thread_safe_queue_test.cpp"
     "tests/worker/worker_test.cpp"
     "tests/worker/worker_pool_test.cpp")

    target_include_directories(distributed_task_platform_tests
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/tests
    )

    target_link_libraries(distributed_task_platform_tests
        PRIVATE
            distributed_task_platform
            GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(distributed_task_platform_tests)
endif()
