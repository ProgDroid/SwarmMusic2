cmake_minimum_required(VERSION 3.10)

project(swarmMusic)

# Set paths and dirs
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")

# Set lib paths for ease of usage
set(GLAD_DIR "${LIB_DIR}/glad")
set(GLFW_DIR "${LIB_DIR}/GLFW")
set(STK_DIR "${LIB_DIR}/stk")

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Set sources
set(SOURCES
    "${SRC_DIR}/main.cpp"
    "${SRC_DIR}/Agent.cpp"
    "${SRC_DIR}/Attractor.cpp"
    "${SRC_DIR}/Scale.cpp"
    "${SRC_DIR}/Swarm.cpp"
    "${SRC_DIR}/Triplet.cpp"
)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_compile_options(-O3 -fomit-frame-pointer -march=native -m64 -Wall -pipe)

include_directories(
    "${INCLUDE_DIR}"
    "${LIB_DIR}"
    "${STK_DIR}/include"
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} pthread)

# glad
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")
target_link_libraries(${PROJECT_NAME} "glad" "${CMAKE_DL_LIBS}")

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
add_subdirectory(${LIB_DIR}/glfw)
target_link_libraries(${PROJECT_NAME} glfw)

# STK
add_definitions(-D__LINUX_ALSA__ -D__LITTLE_ENDIAN__)
add_library(stk STATIC IMPORTED)
set_property(TARGET stk PROPERTY IMPORTED_LOCATION ${STK_DIR}/src/libstk.a)
target_link_libraries(${PROJECT_NAME} stk)
target_link_libraries(${PROJECT_NAME} asound)
target_link_libraries(${PROJECT_NAME} jack)
