cmake_minimum_required(VERSION 3.14)
project(ParallelMonteCarlo VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_program(CLANG_TIDY_EXE NAMES "clang-tidy")

# ==========================================
# 2. Core Library (Движок)
# ==========================================
add_library(CoreEngine
    src/Payoff.cpp
    src/Analytical.cpp
    src/MCEngine.cpp
    src/Payoff.hpp
    src/Analytical.hpp
    src/MCEngine.hpp
    src/Constants.hpp
)

target_include_directories(CoreEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
find_package(Threads REQUIRED)
target_link_libraries(CoreEngine PUBLIC Threads::Threads)

# Подключаем Clang-Tidy к библиотеке
if(CLANG_TIDY_EXE)
    set_target_properties(CoreEngine PROPERTIES 
        CXX_CLANG_TIDY "${CLANG_TIDY_EXE};-warnings-as-errors=*")
endif()

# ==========================================
# 3. Main Executable (CLI)
# ==========================================
add_executable(MonteCarloApp main.cpp)
target_link_libraries(MonteCarloApp PRIVATE CoreEngine)

# Подключаем Clang-Tidy к главному файлу main.cpp
if(CLANG_TIDY_EXE)
    set_target_properties(MonteCarloApp PROPERTIES 
        CXX_CLANG_TIDY "${CLANG_TIDY_EXE};-warnings-as-errors=*")
endif()

# ==========================================
# 4. Benchmark Executable
# ==========================================
add_executable(Benchmark benchmark.cpp)
target_link_libraries(Benchmark PRIVATE CoreEngine)

# ==========================================
# 5. GoogleTest (Unit Tests)
# ==========================================
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# Для Windows: предотвращаем перезапись настроек компилятора
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(UnitTests
    tests/test_analytical.cpp
    tests/test_validation.cpp
    tests/test_monte_carlo.cpp
)

target_link_libraries(UnitTests PRIVATE CoreEngine GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(UnitTests)
