cmake_minimum_required(VERSION 3.5.0)
project(mynteyed_ros2)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()


find_package(MynteyeD REQUIRED)
find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
  message(STATUS "OpenCV is found.")
  set(WITH_OPENCV TRUE)
  add_definitions(-DWITH_OPENCV)
  message(STATUS "Defined WITH_OPENCV.")
endif()


include_directories(
  ${OpenCV_INCLUDE_DIRS}
)

set(LINK_LIBRARIES
  ${OpenCV_LIBS}
  mynteye_depth
)

ament_auto_add_library(mynteye_wrapper_d
  src/mynteye_wrapper_nodelet.cc
  src/pointcloud_generator.cc
)
target_link_libraries(mynteye_wrapper_d ${LINK_LIBRARIES})
#add_dependencies(mynteye_wrapper_d ${PROJECT_NAME}_generate_messages_cpp)

ament_auto_add_executable(mynteye_wrapper_d_node src/mynteye_wrapper_node.cc)
target_link_libraries(mynteye_wrapper_d_node mynteye_wrapper_d ${LINK_LIBRARIES})

ament_auto_add_executable(mynteye_listener_d src/mynteye_listener.cc)
target_link_libraries(mynteye_listener_d ${LINK_LIBRARIES})

# install

install(DIRECTORY launch urdf
  DESTINATION share/${PROJECT_NAME}/
)

ament_auto_package()

