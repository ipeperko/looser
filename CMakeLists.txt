cmake_minimum_required(VERSION 3.10)
project(looser)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if (QTDIR)
    set(Qt5_DIR ${QTDIR})
    set(CMAKE_PREFIX_PATH "$ENV{QTDIR}")
endif(QTDIR)

message("QTDIR HINT=${QTDIR}")
message("Qt5_DIR=${Qt5_DIR}")

find_package(Qt5 COMPONENTS Widgets REQUIRED)

set (LIBS PRIVATE Qt5::Widgets dbm)
file (GLOB SOURCES *.cpp )
file (GLOB HEADERS *.h)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} ${LIBS})