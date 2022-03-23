# CMakeList.txt : CMake project for ThreadPool, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)



project ("ThreadPool")

# Add source to this project's executable.
add_executable (ThreadPool "program.cpp" "ThreadPool.cpp" "ThreadPool.h" "ConcurrentQueue.cpp" "ConcurrentQueue.h")


# TODO: Add tests and install targets if needed.
