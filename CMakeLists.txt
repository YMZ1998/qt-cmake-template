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

set(QT_HOME "D:/Software/QT/5.12.12/msvc2017_64")
# Qt's package files live below the compiler kit directory, for example:
# ${QT_HOME}/lib/cmake/Qt5/Qt5Config.cmake
set(QT_PACKAGE_HINTS)
if(DEFINED QT_HOME)
    message(STATUS "Looking for Qt in: ${QT_HOME}")
    list(APPEND QT_PACKAGE_HINTS
        "${QT_HOME}"
        "${QT_HOME}/lib/cmake/Qt5")
elseif(DEFINED Qt${QT_MAJOR_VERSION}_DIR)
    list(APPEND QT_PACKAGE_HINTS "${Qt${QT_MAJOR_VERSION}_DIR}")
endif()

if(NOT DEFINED QT_HOME AND DEFINED ENV{Qt${QT_MAJOR_VERSION}_HOME})
    set(QT_HOME "$ENV{Qt${QT_MAJOR_VERSION}_HOME}")
    message(STATUS "Looking for Qt in: ${QT_HOME}")
    list(APPEND QT_PACKAGE_HINTS
        "${QT_HOME}"
        "${QT_HOME}/msvc2017_64/lib/cmake/Qt5"
        "${QT_HOME}/msvc2015_64/lib/cmake/Qt5"
        "${QT_HOME}/msvc2019_64/lib/cmake/Qt5")
else()
    message(STATUS "Qt${QT_MAJOR_VERSION}_HOME environment variable not set. Checking default paths.")
endif()

find_package(Qt${QT_MAJOR_VERSION} CONFIG REQUIRED
    COMPONENTS Widgets Multimedia MultimediaWidgets
    PATHS ${QT_PACKAGE_HINTS})

add_executable(${TARGET_NAME}
    src/window.h
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
    if(NOT DEFINED QT_HOME AND DEFINED Qt${QT_MAJOR_VERSION}_DIR)
        get_filename_component(QT_HOME
            "${Qt${QT_MAJOR_VERSION}_DIR}/../../.."
            ABSOLUTE)
    endif()

    if(EXISTS "${QT_HOME}/bin/Qt5Core.dll")
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND "${CMAKE_COMMAND}"
                -DQT_HOME=${QT_HOME}
                -DDEPLOY_DIR=$<TARGET_FILE_DIR:${TARGET_NAME}>
                -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/DeployQtRuntime.cmake"
            COMMENT "Copying Qt runtime and plugins")
    else()
        message(WARNING
            "Qt runtime was not found under QT_HOME. Expected "
            "${QT_HOME}/bin/Qt5Core.dll.")
    endif()
endif()
