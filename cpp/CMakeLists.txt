cmake_minimum_required(VERSION 3.25)
project(cpp-scrap CXX)

find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
    add_executable(app
    ./src/main.cpp
    ./src/clahe.cpp
    )
    target_link_libraries(app PRIVATE ${OpenCV_LIBS})
    target_include_directories(app PRIVATE "./include")
endif(OpenCV_FOUND)
