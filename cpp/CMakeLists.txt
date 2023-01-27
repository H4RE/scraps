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

    include(FetchContent)
    FetchContent_Declare(
    googletest
    # Specify the commit you depend on and update it regularly.
    URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    # Now simply link against gtest or gtest_main as needed. Eg
    add_executable(clahe-test 
    ./src/clahe.cpp
    ./test/clahe_test.cpp
    )
    target_link_libraries(clahe-test PRIVATE gtest_main ${OpenCV_LIBS})
    target_include_directories(clahe-test PRIVATE ./include/)

    enable_testing()
    add_test(NAME test COMMAND clahe-test.exe "../inu.jpg")

endif(OpenCV_FOUND)
