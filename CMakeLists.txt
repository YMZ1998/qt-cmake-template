cmake_minimum_required(VERSION 3.16)

#Project name
set(TARGET_NAME VideoBrowser)

#You can change this to Qt5 if you
#are not ready to migrate to Qt6.
set(QT_MAJOR_VERSION 5)

project(${TARGET_NAME} VERSION 0.0.1 DESCRIPTION "Project Description")

set(PROJECT_OUTPUT_DIR "${PROJECT_BINARY_DIR}/../output")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_OUTPUT_DIR} )
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_OUTPUT_DIR}")
set(LIBRARY_OUTPUT_PATH "${PROJECT_OUTPUT_DIR}")

#Make sure CMake will take care of moc for us
set(CMAKE_AUTOMOC ON)

#Set the CXX standard. Qt 6 I believe uses c++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#See if the environment var is set
if(DEFINED ENV{Qt${QT_MAJOR_VERSION}_HOME})
    message(STATUS "Looking for Qt in: " $ENV{Qt${QT_MAJOR_VERSION}_HOME})
else()
    message(STATUS "Qt${QT_MAJOR_VERSION}_HOME environment variable not set. Checking default paths.")
endif()

#find_package should find everything fine so long as the ENV Variable is set or, for linux systems,
#it is in the default install path.
find_package(Qt${QT_MAJOR_VERSION} COMPONENTS Widgets Multimedia MultimediaWidgets REQUIRED PATHS $ENV{Qt${QT_MAJOR_VERSION}_HOME})

add_executable(${TARGET_NAME}
    include/window.hpp
    src/window.cpp
    src/main.cpp)
    
if(MSVC)
    target_compile_options(${TARGET_NAME} PRIVATE /W4 /WX /utf-8)
else()
    target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

target_include_directories(${TARGET_NAME} PRIVATE ${Qt${QT_MAJOR_VERSION}_INCLUDE_DIRS})
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

set_target_properties(${TARGET_NAME} PROPERTIES DEBUG_OUTPUT_NAME "${TARGET_NAME}d" RELEASE_OUTPUT_NAME ${TARGET_NAME})

#Add any extra libs to link also.
target_link_libraries(${TARGET_NAME} PRIVATE
    Qt${QT_MAJOR_VERSION}::Widgets
    Qt${QT_MAJOR_VERSION}::Multimedia
    Qt${QT_MAJOR_VERSION}::MultimediaWidgets)

if(WIN32)
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:${TARGET_NAME}>/mediaservice"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${Qt${QT_MAJOR_VERSION}_DIR}/../../../plugins/mediaservice/wmfengine.dll"
            "$<TARGET_FILE_DIR:${TARGET_NAME}>/mediaservice/wmfengine.dll"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${Qt${QT_MAJOR_VERSION}_DIR}/../../../plugins/mediaservice/dsengine.dll"
            "$<TARGET_FILE_DIR:${TARGET_NAME}>/mediaservice/dsengine.dll"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${Qt${QT_MAJOR_VERSION}_DIR}/../../../plugins/mediaservice/qtmedia_audioengine.dll"
            "$<TARGET_FILE_DIR:${TARGET_NAME}>/mediaservice/qtmedia_audioengine.dll")
endif()
