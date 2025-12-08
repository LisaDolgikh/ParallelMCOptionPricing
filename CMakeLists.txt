cmake_minimum_required(VERSION 3.14)
project(ParallelMonteCarlo VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Ищем Clang-Tidy
find_program(CLANG_TIDY_EXE NAMES "clang-tidy")

# ==========================================
# 2. Core Library
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

# ==========================================
# 3. Main Executable (CLI)
# ==========================================
add_executable(MonteCarloApp main.cpp)
target_link_libraries(MonteCarloApp PRIVATE CoreEngine)

# ==========================================
# 4. ПРИНУДИТЕЛЬНАЯ ПРОВЕРКА КОДА
# ==========================================
if(CLANG_TIDY_EXE)
    message(STATUS "Clang-Tidy found, enforcing strict checks via arguments...")
    
    set(DO_CHECK 
        "${CLANG_TIDY_EXE}"
        "-checks=-*,google-readability-casting,cppcoreguidelines-pro-type-cstyle-cast"
        "-warnings-as-errors=*"
    )
    
    # Вешаем проверку на исполняемый файл
    set_target_properties(MonteCarloApp PROPERTIES CXX_CLANG_TIDY "${DO_CHECK}")
    
    # Вешаем проверку на библиотеку
    set_target_properties(CoreEngine PROPERTIES CXX_CLANG_TIDY "${DO_CHECK}")
endif()

# ==========================================
# 5. Benchmark
# ==========================================
add_executable(Benchmark benchmark.cpp)
target_link_libraries(Benchmark PRIVATE CoreEngine)

# ==========================================
# 6. GoogleTest
# ==========================================
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
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