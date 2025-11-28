add_library(CoreEngine
    Payoff.cpp
    Analytical.cpp
    MCEngine.cpp
    # Constants.hpp
)

# Экспортируем include директории, чтобы тесты их видели
target_include_directories(CoreEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Threads REQUIRED)
target_link_libraries(CoreEngine PUBLIC Threads::Threads)