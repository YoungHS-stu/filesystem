cmake_minimum_required(VERSION 3.0.0)
project(YoungFileSystem VERSION 0.1.0)

add_executable(YFS main.cpp)

ADD_SUBDIRECTORY(src/Disk)
#ADD_SUBDIRECTORY(src/FileManager)
#ADD_SUBDIRECTORY(src/Shell)
