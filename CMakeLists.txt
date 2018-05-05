cmake_minimum_required(VERSION 3.0)

project(posixthread)

set( CMAKE_VERBOSE_MAKEFILE on )

set (CMAKE_C_FLAGS "-fPIC ${CMAKE_C_FLAGS}")
set (CMAKE_CXX_FLAGS "-std=c++11 -fPIC -Wall -O2 -pipe -g -feliminate-unused-debug-types  -fstack-protector-strong -pie -fpie -D_FORTIFY_SOURCE=2 -Wformat -Wformat-security -fvisibility-inlines-hidden ${CMAKE_CXX_FLAGS}")

# Tell CMake to create the executable
add_executable(posixthread_mq_nonblock posixthread_mq_nonblock.cpp)

target_link_libraries (posixthread_mq_nonblock pthread)
target_link_libraries (posixthread_mq_nonblock rt)

add_executable(posixthread_mq_blocking posixthread_mq_blocking.cpp)

target_link_libraries (posixthread_mq_blocking pthread)
target_link_libraries (posixthread_mq_blocking rt)

add_executable(posixthread_semaphore_mutex_example posixthread_semaphore_mutex_example.cpp)

target_link_libraries (posixthread_semaphore_mutex_example pthread)
target_link_libraries (posixthread_semaphore_mutex_example rt)

add_executable(posixthread_semaphore_sync_example posixthread_semaphore_sync_example.cpp)

target_link_libraries (posixthread_semaphore_sync_example pthread)
target_link_libraries (posixthread_semaphore_sync_example rt)

add_executable(signal_handling signal_handling.cpp)

add_executable(pipe_example pipe_example.cpp)

target_link_libraries (pipe_example pthread)




