cmake_minimum_required(VERSION 3.17)
project(HandyFramework)

file(GLOB_RECURSE HandyFramework_HEADERS "${PROJECT_SOURCE_DIR}/src/**.hpp")
file(GLOB_RECURSE HandyFramework_SOURCES "${PROJECT_SOURCE_DIR}/src/**.cpp")

add_library(HandyFramework STATIC ${HandyFramework_HEADERS} ${HandyFramework_SOURCES})
set_target_properties(HandyFramework PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_link_libraries(HandyFramework PUBLIC
    "fmt"
    "dl" # for loguru
    "pthread" # for loguru
)
