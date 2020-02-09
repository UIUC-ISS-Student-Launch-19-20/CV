cmake_minimum_required(VERSION 2.8)
project( squares )

find_package( OpenCV REQUIRED )
find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  sensor_msgs
  cv_bridge
  image_transport
  std_msgs
)

include_directories(
    ${catkin_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS})

add_executable( squares OpenCV.cpp )
target_link_libraries( squares ${OpenCV_LIBS} ${catkin_LIBRARIES})
