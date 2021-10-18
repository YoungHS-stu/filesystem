cmake_minimum_required(VERSION 3.0.0)
project(Disk VERSION 0.1.0)

include_directories(
    "../"
    "./"
    "../Disk"
)

aux_source_directory(. SOURCE)
aux_source_directory(../Disk SOURCE)
#list(REMOVE_ITEM SOURCE "Itemp.cpp")
add_executable(FileManager ${SOURCE})

