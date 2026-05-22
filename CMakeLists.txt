cmake_minimum_required(VERSION 3.5...3.28)

project(champ)

set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Default build type" FORCE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DNDEBUG")
    if(MSVC)
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /DEBUG:NONE")
    else()
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -g0")
    endif()
endif()

file(GLOB_RECURSE SOURCES
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

include_directories(
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/include/Champ"
    "${PROJECT_SOURCE_DIR}/include/Champ/Core"
)

# GLFW settings
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")

add_subdirectory(libs/GLFW)
add_subdirectory(libs/glad)
add_subdirectory(libs/imgui)
add_subdirectory(libs/stb)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE glfw glad imgui stb)

target_include_directories(${PROJECT_NAME} PUBLIC 
   ${PROJECT_SOURCE_DIR}/libs/GLFW/include
   ${PROJECT_SOURCE_DIR}/libs/glad/include
   ${PROJECT_SOURCE_DIR}/libs/imgui/include
   ${PROJECT_SOURCE_DIR}/libs/glm/include
   ${PROJECT_SOURCE_DIR}/libs/stb/include
)
