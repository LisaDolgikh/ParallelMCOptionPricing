cmake_minimum_required(VERSION 3.14)
project(ParallelMonteCarlo VERSION 1.0 LANGUAGES CXX)

# ==========================================
# FORCE CLANG-TIDY INTEGRATION
# ==========================================
# Ищем программу clang-tidy
find_program(CLANG_TIDY_EXE NAMES "clang-tidy")

# Если нашли - включаем принудительно для всех C++ файлов
if(CLANG_TIDY_EXE)
    message(STATUS "Run: Clang-Tidy found at ${CLANG_TIDY_EXE}, enabling checks...")
    # ; -warnings-as-errors=* дублирует настройку из файла, для надежности
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE};-warnings-as-errors=*")
else()
    message(WARNING "Clang-Tidy NOT found! Code analysis disabled.")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ==========================================
# 1. Core Library (Движок)
# ==========================================
add_library(CoreEngine
    src/Payoff.cpp
    src/Analytical.cpp
    src/MCEngine.cpp
    # Header files included for IDE visibility
    src/Payoff.hpp
    src/Analytical.hpp
    src/MCEngine.hpp
    src/Constants.hpp
)

target_include_directories(CoreEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Threads REQUIRED)
target_link_libraries(CoreEngine PUBLIC Threads::Threads)

# ==========================================
# 2. Main Executable (CLI)
# ==========================================
add_executable(MonteCarloApp main.cpp)
target_link_libraries(MonteCarloApp PRIVATE CoreEngine)

# ==========================================
# 3. Benchmark Executable
# ==========================================
add_executable(Benchmark benchmark.cpp)
target_link_libraries(Benchmark PRIVATE CoreEngine)

# ==========================================
# 4. GoogleTest (Unit Tests)
# ==========================================
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Собираем тесты
add_executable(UnitTests
    tests/test_analytical.cpp
    tests/test_validation.cpp
    tests/test_monte_carlo.cpp
)

target_link_libraries(UnitTests PRIVATE CoreEngine GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(UnitTests)
