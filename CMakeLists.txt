cmake_minimum_required(VERSION 3.17)
project(BPSK)

set(CMAKE_CXX_STANDARD 14)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
add_executable(BPSK main.cpp)
target_link_libraries(BPSK PRIVATE Threads::Threads)