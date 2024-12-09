cmake_minimum_required(VERSION 3.10)

project(KIHFilt)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(nlohmann_json_DIR "C:/Users/HP/vcpkg/installed/x64-windows/share/nlohmann_json")

find_package(nlohmann_json CONFIG REQUIRED)

add_executable(KIHFilt filtration.cpp)

target_link_libraries(KIHFilt PRIVATE nlohmann_json::nlohmann_json)
