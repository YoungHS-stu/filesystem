cmake_minimum_required(VERSION 3.0.0)
project(Disk VERSION 0.1.0)

include_directories(
    ../
    ./
)

aux_source_directory(. SOURCE)
#list(REMOVE_ITEM SOURCE "Itemp.cpp")
add_executable(DiskT ${SOURCE})
