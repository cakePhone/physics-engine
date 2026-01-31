cmake_minimum_required(VERSION 3.10)
project(PhysicsEngine)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # This is the magic line for Neovim/clangd

# Find dependencies
find_package(raylib REQUIRED)
find_package(cglm REQUIRED)

add_executable(${PROJECT_NAME} src/main.c)
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Link libraries
target_link_libraries(${PROJECT_NAME} raylib cglm m)
