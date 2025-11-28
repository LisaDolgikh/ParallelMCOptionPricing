# Создаем исполняемый файл для тестов
add_executable(UnitTests main_test.cpp)

# Линкуем с нашей библиотекой CoreEngine и библиотекой GTest
target_link_libraries(UnitTests PRIVATE 
    CoreEngine 
    gtest_main  # Включает в себя main() для тестов
)

# Включаем поддержку CTest
include(GoogleTest)
gtest_discover_tests(UnitTests)