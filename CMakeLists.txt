cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(satellite-plot)

add_compile_options(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
  roscpp
)

find_package(OpenCV REQUIRED)

include_directories(
  /home/uchiito-h/catkin_ws/devel/include/
  ${catkin_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
)

add_executable (satellite-plot src/main.cpp src/ocam_model.cpp src/util.cpp src/read_dump_files.cpp src/draw_satellites.cpp src/satellite_plot.cpp)
target_link_libraries (satellite-plot ${OpenCV_LIBS})
add_dependencies(satellite-plot ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

target_link_libraries(satellite-plot
  ${catkin_LIBRARIES}
  ${OpenCV_LIBRARIES}
)