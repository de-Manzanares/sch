if (SCH_ENABLE_TESTS)

    include(CTest)
    include(FetchContent)

    FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.8.0
    )
    FetchContent_MakeAvailable(Catch2)

    add_executable(BigInt-test BigInt.cxx ${CMAKE_SOURCE_DIR}/include/BigInt.hpp)
    target_include_directories(BigInt-test PRIVATE ../include)
    target_link_libraries(BigInt-test PRIVATE Catch2::Catch2WithMain)
    target_compile_options(BigInt-test PRIVATE
            $<$<CXX_COMPILER_ID:GNU,Clang>:--coverage -Wall -Wextra -Wpedantic>
            $<$<CXX_COMPILER_ID:MSVC>:/W4>
    )
    target_link_options(BigInt-test PRIVATE
            $<$<CXX_COMPILER_ID:GNU,Clang>:--coverage -lgcov>
    )

    add_executable(BigInt-perf-test BigInt_perf.cxx ${CMAKE_SOURCE_DIR}/include/BigInt.hpp)
    target_include_directories(BigInt-perf-test PRIVATE ../include)
    target_link_libraries(BigInt-perf-test PRIVATE Catch2::Catch2WithMain)
    target_compile_options(BigInt-perf-test PRIVATE
            $<$<CXX_COMPILER_ID:GNU,Clang>:--coverage -Wall -Wextra -Wpedantic>
            $<$<CXX_COMPILER_ID:MSVC>:/W4>
    )
    target_link_options(BigInt-perf-test PRIVATE
            $<$<CXX_COMPILER_ID:GNU,Clang>:--coverage -lgcov>
    )

    add_test(NAME BigInt-test COMMAND BigInt-test)
    set_tests_properties(BigInt-test PROPERTIES LABELS unit)
    add_test(NAME BigInt-perf-test COMMAND BigInt-perf-test)
    set_tests_properties(BigInt-perf-test PROPERTIES LABELS perf)

endif ()
