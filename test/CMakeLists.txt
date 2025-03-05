include(CTest)

find_package(Catch2 3 REQUIRED)

add_executable(BigUInt-test BigUInt.cxx ${CMAKE_SOURCE_DIR}/src/BigUInt.hpp)
target_include_directories(BigUInt-test PRIVATE ../src)
target_link_libraries(BigUInt-test PRIVATE Catch2::Catch2WithMain gcov)
target_compile_options(BigUInt-test PRIVATE --coverage -Wall -Wextra -Wpedantic)
target_link_options(BigUInt-test PRIVATE --coverage -lgcov)

add_test(NAME BigUInt-test COMMAND BigUInt-test)
