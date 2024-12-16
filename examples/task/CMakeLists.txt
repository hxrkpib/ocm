cmake_minimum_required(VERSION 3.14)

project(TaskTest LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 追加新的路径到 CMAKE_PREFIX_PATH
set(CMAKE_PREFIX_PATH "/opt/openrobotlib/ocm" "/opt/openrobotlib/third_party" "/opt/ros/jazzy")
# 输出 CMAKE_PREFIX_PATH 进行调试
message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")

# 查找 OCM 包
find_package(OCM REQUIRED)

# 添加可执行文件
add_executable(Trigger trigger.cpp)
add_executable(InternalTimerTest internal_timer.cpp)
add_executable(ExternalTimerTest external_timer.cpp)
target_link_libraries(Trigger PUBLIC OCM::OCM)
target_link_libraries(InternalTimerTest PUBLIC OCM::OCM)
target_link_libraries(ExternalTimerTest PUBLIC OCM::OCM)
