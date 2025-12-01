add_executable(UnitTests 
    test_analytical.cpp
    test_validation.cpp
    test_monte_carlo.cpp
)

# Линкуем с нашей библиотекой CoreEngine и библиотекой GTest
target_link_libraries(UnitTests PRIVATE 
    CoreEngine 
    gtest_main  # Включает в себя main() для тестов
)

# Включаем поддержку CTest
include(GoogleTest)
gtest_discover_tests(UnitTests)