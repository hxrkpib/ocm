cmake_minimum_required(VERSION 3.5)
if(POLICY CMP0111)
  cmake_policy(SET CMP0111 NEW)
endif()
project(
  OCM
  VERSION 1.0
  DESCRIPTION "One Communication Middleware"
  LANGUAGES CXX)

if(NOT DEFINED SUPPORT_ROS2)
  set(SUPPORT_ROS2 OFF CACHE BOOL "Enable or disable ROS 2 support" FORCE)
endif()

if(SUPPORT_ROS2)
  if(NOT ROS_DISTRO OR ROS_DISTRO STREQUAL "")
    message(FATAL_ERROR "Error: ROS_DISTRO is empty!")
  else()
    message(STATUS "ROS_DISTRO is: ${ROS_DISTRO}")
  endif()
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  message(
    STATUS
      "CMAKE_INSTALL_PREFIX is using the default value, changing to '/opt/openrobotlib/ocm'"
  )
  set(CMAKE_INSTALL_PREFIX
      "/opt/openrobotlib/ocm"
      CACHE PATH "Default install path" FORCE)
else()
  message(STATUS "CMAKE_INSTALL_PREFIX already set to: ${CMAKE_INSTALL_PREFIX}")
endif()

# 输出最终安装路径
message(STATUS "Final CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
if(NOT DEFINED CMAKE_PREFIX_PATH OR CMAKE_PREFIX_PATH STREQUAL "")
  set(CMAKE_PREFIX_PATH
      "/opt/openrobotlib/third_party"
      "/opt/ros/${ROS_DISTRO}"
      CACHE PATH "Default prefix path" FORCE)
  message(
    STATUS
      "CMAKE_PREFIX_PATH is not set, defaulting to '/opt/openrobotlib/third_party'"
  )
else()
  message(STATUS "CMAKE_PREFIX_PATH already set to: ${CMAKE_PREFIX_PATH}")
endif()

# 输出最终前缀路径
message(STATUS "Final CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(lcm REQUIRED)
find_package(spdlog REQUIRED)
find_package(yaml-cpp REQUIRED)
if(SUPPORT_ROS2)
  find_package(rclcpp REQUIRED)
  find_package(rosidl_typesupport_cpp REQUIRED)
  find_package(rosidl_typesupport_fastrtps_cpp REQUIRED)
  find_package(rosidl_default_generators REQUIRED)
  find_package(rosidl_runtime_c REQUIRED)
endif()


add_compile_options(-no-pie -fPIC -O3 -march=native)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

file(GLOB_RECURSE OCM_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_library(OCM SHARED ${OCM_SOURCES})

# 1. 指定头文件路径
target_include_directories(
  OCM PUBLIC 
  ${rosidl_typesupport_cpp_INCLUDE_DIRS}
  ${rosidl_typesupport_fastrtps_cpp_INCLUDE_DIRS}
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include> # 安装时的头文件路径
)



# 1. 链接外部库
if(SUPPORT_ROS2)
  target_link_libraries(OCM PUBLIC ${LCM_NAMESPACE}lcm spdlog::spdlog
                                 yaml-cpp::yaml-cpp rclcpp::rclcpp ${rosidl_typesupport_cpp_LIBRARIES}
                                 ${rosidl_typesupport_fastrtps_cpp_LIBRARIES}
                                 ${rosidl_runtime_c_LIBRARIES})
else()
  target_link_libraries(OCM PUBLIC ${LCM_NAMESPACE}lcm spdlog::spdlog
                                 yaml-cpp::yaml-cpp)
endif()
# 1. 安装头文件
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include)

# 1. 安装库文件
install(
  TARGETS OCM
  EXPORT OCMTargets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  INCLUDES
  DESTINATION include)

# 1. 导出目标信息
install(
  EXPORT OCMTargets
  FILE OCMTargets.cmake
  NAMESPACE OCM::
  DESTINATION lib/cmake/OCM)

# 1. 配置并安装包配置文件
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/OCMConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion)

if(SUPPORT_ROS2)
  configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/OCMConfigRos.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/OCMConfig.cmake"
    INSTALL_DESTINATION lib/cmake/OCM
  )
else()
  configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/OCMConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/OCMConfig.cmake"
    INSTALL_DESTINATION lib/cmake/OCM
  )
endif()

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/OCMConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/OCMConfigVersion.cmake"
        DESTINATION lib/cmake/OCM)

# 设置源文件路径
set(SOURCE_FILE
    "${CMAKE_SOURCE_DIR}/python/parameter_generator/dist/ocm-parmgen")

# 设置目标文件路径
set(TARGET_PATH "/usr/local/bin/")

# 创建一个自定义命令来复制文件
add_custom_command(
  TARGET OCM
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE} ${TARGET_PATH}
  COMMENT "Copying ocm-parmgen to /usr/local/bin/")
