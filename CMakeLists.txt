cmake_minimum_required(VERSION 3.20)
project(ParallelMCOptionPricing VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Флаги предупреждений (Industrial Quality)
if(MSVC)
    add_compile_options(/W4 /WX /permissive-)
else()
    add_compile_options(-Wall -Wextra -Werror -pedantic -O3 -march=native)
endif()

# Подключение GoogleTest (через FetchContent — самый надежный способ)
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
# Для предотвращения установки gtest в системные папки
set(INSTALL_GTEST OFF) 
FetchContent_MakeAvailable(googletest)

# Подключаем поддиректории
add_subdirectory(src)
add_subdirectory(tests)

# Основной исполняемый файл (для демо)
add_executable(OptionPricer main.cpp)
target_link_libraries(OptionPricer PRIVATE CoreEngine)