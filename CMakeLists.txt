cmake_minimum_required(VERSION 3.30)
project(Statemachines)

# add gtest
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(CMAKE_CXX_STANDARD 20)
# force standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(statemachine_lib INTERFACE lib/statemachine.h)
target_include_directories(statemachine_lib INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib>
        $<INSTALL_INTERFACE:include>)

enable_testing()
add_executable(statemachine_test tests/statemachine_tests.cpp)
target_link_libraries(statemachine_test gtest gtest_main statemachine_lib)

include(GoogleTest)
gtest_discover_tests(statemachine_test)