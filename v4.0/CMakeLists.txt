cmake_minimum_required(VERSION 3.0)

project(NDSE_Server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(Protobuf_ROOT /usr/local)
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -g")

set(SOURCE_FILES 
    server/main.cpp
    server/utilities.cpp
    server/server.cpp
    server/recommend.cpp
    server/websearch.cpp
    server/semantic_search.grpc.pb.cc
    server/semantic_search.pb.cc
    server/semantic.cpp
)

find_package(Protobuf REQUIRED CONFIG PATHS /usr/local/lib/cmake/protobuf)
find_package(gRPC REQUIRED CONFIG PATHS /usr/local/lib/cmake/grpc)

# 包含protobuf头文件目录
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${gRPC_INCLUDE_DIRS})

add_executable(server ${SOURCE_FILES})

target_link_libraries(server workflow wfrest tinyxml2 protobuf::libprotobuf gRPC::grpc++ pthread z)