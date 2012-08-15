project(TriangleRender)

cmake_minimum_required(VERSION 2.4.0)

# show all warnings
set(CMAKE_CXX_FLAGS "-Wall")

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
set(LIBS ${LIBS} ${OPENGL_LIBRARIES})

find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIR})
set(LIBS ${LIBS} ${GLUT_LIBRARIES})

set(SRCS ${SRCS}
    src/main.cpp
)

add_executable(trianglerender ${SRCS})

target_link_libraries(trianglerender ${LIBS})
