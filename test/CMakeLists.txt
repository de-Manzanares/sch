include(CTest)

find_package(Catch2 3 REQUIRED)

add_executable(BigInt-test BigInt.cxx ${CMAKE_SOURCE_DIR}/include/BigInt.hpp)
target_include_directories(BigInt-test PRIVATE ../include)
target_link_libraries(BigInt-test PRIVATE Catch2::Catch2WithMain gcov)
target_compile_options(BigInt-test PRIVATE --coverage -Wall -Wextra -Wpedantic)
target_link_options(BigInt-test PRIVATE --coverage -lgcov)

add_test(NAME BigInt-test COMMAND BigInt-test)
