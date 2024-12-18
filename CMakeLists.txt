cmake_minimum_required(VERSION 3.10)
project(ExpenseManager)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static -lpthread")

include_directories(${CMAKE_SOURCE_DIR}/sqlite)

add_library(sqlite3 STATIC sqlite/sqlite3.c)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} sqlite3)
