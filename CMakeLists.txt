cmake_minimum_required(VERSION 3.17)
project(HW3)

SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS -pthread)

add_executable(HW3-p1-t1 Tests/ts1.cpp Code_Skeleton/Part1/Semaphore.hpp Code_Skeleton/Part1/Semaphore.cpp Code_Skeleton/Part1/Headers.hpp Code_Skeleton/Part1/PCQueue.hpp)
add_executable(HW3-p1-t2 Tests/ts2.cpp Code_Skeleton/Part1/Semaphore.hpp Code_Skeleton/Part1/Semaphore.cpp Code_Skeleton/Part1/Headers.hpp Code_Skeleton/Part1/PCQueue.hpp)
add_executable(HW3-p1-t3 Tests/ts3.cpp Code_Skeleton/Part1/Semaphore.hpp Code_Skeleton/Part1/Semaphore.cpp Code_Skeleton/Part1/Headers.hpp Code_Skeleton/Part1/PCQueue.hpp)
#add_executable(HW3-p2-t1 Code_Skeleton/Part2/main.cpp Code_Skeleton/Part2/Game.cpp Code_Skeleton/Part2/utils.cpp Code_Skeleton/Part2/utils.hpp Code_Skeleton/Part2/Thread.hpp Code_Skeleton/Part1/Semaphore.hpp Code_Skeleton/Part1/Semaphore.cpp Code_Skeleton/Part1/Headers.hpp Code_Skeleton/Part1/PCQueue.hpp)
add_executable(HW3-p2-t1 Code_Skeleton/Part2/main.cpp Code_Skeleton/Part2/Game.cpp Code_Skeleton/Part2/utils.cpp Code_Skeleton/Part2/utils.hpp Code_Skeleton/Part2/Thread.hpp Code_Skeleton/Part1/Semaphore.hpp Code_Skeleton/Part1/Semaphore.cpp Code_Skeleton/Part1/Headers.hpp Code_Skeleton/Part1/PCQueue.hpp Code_Skeleton/Part2/WorkingThread.cpp Code_Skeleton/Part2/WorkingThread.h)
add_executable(HW3-p2-test-shachar tester0/run_test0.cpp Code_Skeleton/Part2/Game.cpp Code_Skeleton/Part2/utils.cpp Code_Skeleton/Part2/utils.hpp Code_Skeleton/Part2/Thread.hpp Code_Skeleton/Part1/Semaphore.hpp Code_Skeleton/Part1/Semaphore.cpp Code_Skeleton/Part1/Headers.hpp Code_Skeleton/Part1/PCQueue.hpp Code_Skeleton/Part2/WorkingThread.cpp Code_Skeleton/Part2/WorkingThread.h)