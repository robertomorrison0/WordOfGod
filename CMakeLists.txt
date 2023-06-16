set(CMAKE_VERBOSE_MAKEFILE ON)
cmake_minimum_required(VERSION 3.12)
project(org.robertomorrison.biblebolls VERSION 0.0.1)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_GENERATOR Ninja)


find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk4)
pkg_check_modules(ADW REQUIRED libadwaita-1)
pkg_check_modules(JSON REQUIRED jsoncpp)
pkg_check_modules(CURL REQUIRED libcurl)


set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(LIBRARY_DIR ${PROJECT_SOURCE_DIR}/lib)
set(RESOURCE_DIR ${PROJECT_SOURCE_DIR}/resources)

file(GLOB SOURCESCPP ${LIBRARY_DIR}/*.cpp)
file(GLOB SOURCESC ${LIBRARY_DIR}/*.c)
file(GLOB RESOURCESC ${RESOURCE_DIR}/*.c)


add_executable(org.robertomorrison.biblebolls main.cpp ${RESOURCESC} ${SOURCESC} ${SOURCESCPP} ) #${CMAKE_CURRENT_BINARY_DIR}/resources.c )


target_include_directories(org.robertomorrison.biblebolls PRIVATE ${INCLUDE_DIR} ${GTK_INCLUDE_DIRS} ${ADW_INCLUDE_DIRS} ${JSON_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS})
target_compile_options(org.robertomorrison.biblebolls PUBLIC ${GTK_CFLAGS_OTHER} ${ADW_CFLAGS_OTHER} ${JSON_CFLAGS_OTHER} ${CURL_CFLAGS_OTHER})

target_link_libraries(org.robertomorrison.biblebolls ${GTK_LIBRARIES} ${ADW_LIBRARIES} ${JSON_LIBRARIES} ${CURL_LIBRARIES})