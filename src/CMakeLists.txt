cmake_minimum_required(VERSION 3.5)

project(video_thumbnail_generator LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)

#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS MultimediaWidgets REQUIRED)

add_executable(sample sample.cpp video_thumbnail_generator.cpp offscreen_video_surface.cpp)
target_link_libraries(sample PRIVATE Qt5::MultimediaWidgets)

########################################################
# TESTS                                                #
########################################################
enable_testing()
find_package(Qt5Test REQUIRED)
add_executable(TestVideoThumbnailGenerator test_video_thumbnail_generator.cpp video_thumbnail_generator.cpp offscreen_video_surface.cpp)
target_link_libraries(TestVideoThumbnailGenerator PRIVATE Qt5::Test Qt5::MultimediaWidgets)
add_test(NAME TestVideoThumbnailGenerator COMMAND TestVideoThumbnailGenerator)


