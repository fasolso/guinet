cmake_minimum_required(VERSION 3.12)

set(BUILD_STATIC FALSE)

project(libguinet VERSION 1.0.0.1)

file(GLOB libwebes_SRC CONFIGURE_DEPENDS "./src/*.cxx" 
                                        "./include/*.hxx")

message("BUILD STATIC " ${libwebes_SRC})

if (BUILD_STATIC)
    message("Building static library " ${PROJECT_NAME})
    add_library(${PROJECT_NAME} STATIC ${libwebes_SRC})
endif()

if(NOT BUILD_STATIC)
    message("Building dynamic library " ${PROJECT_NAME})
    add_library(${PROJECT_NAME}${VERSION} SHARED ${libwebes_SRC})
endif()

include_directories(${PROJECT_NAME} "include/")

add_executable("test" "test/src/main.cxx")
target_compile_options("test" PUBLIC -std=c++2a PUBLIC -fpermissive PUBLIC -fconcepts)
target_link_libraries("test" webes curlpp)
target_include_directories("test" PRIVATE "./test/include/")

include("cmake/test.cmake")
