cmake_minimum_required(VERSION 3.14)
project(PhysicsEngine)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find dependencies
find_package(raylib REQUIRED)
find_package(cglm REQUIRED)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
add_executable(${PROJECT_NAME} ${SRC_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Link libraries
target_link_libraries(${PROJECT_NAME} raylib cglm m)
