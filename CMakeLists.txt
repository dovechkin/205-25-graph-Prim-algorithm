cmake_minimum_required(VERSION 3.12)
project(PrimAlgorithm VERSION 1.0.0)

# Настройка стандарта C++
set(CMAKE_CXX_STANDARD 17)

# Загрузка библиотек
include(FetchContent)
FetchContent_Declare(httplib GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git GIT_TAG v0.20.1)
FetchContent_Declare(json GIT_REPOSITORY https://github.com/nlohmann/json.git GIT_TAG v3.12.0)
FetchContent_MakeAvailable(httplib json)

# Создание исполняемого файла
add_executable(server src/handler.cpp src/server.cpp)
add_executable(client src/client.cpp)

include_directories(include)

target_include_directories(server PRIVATE 
    ${CMAKE_BINARY_DIR}/_deps/
    ${CMAKE_BINARY_DIR}/_deps/httplib-src/
    ${CMAKE_BINARY_DIR}/_deps/json-src/single_include/nlohmann/
)

target_include_directories(client PRIVATE 
    ${CMAKE_BINARY_DIR}/_deps/
    ${CMAKE_BINARY_DIR}/_deps/httplib-src/
    ${CMAKE_BINARY_DIR}/_deps/json-src/single_include/nlohmann/
)

file(COPY ${CMAKE_SOURCE_DIR}/test/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})