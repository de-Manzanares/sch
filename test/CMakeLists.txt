if (SCH_ENABLE_TESTS)

    include(CTest)
    include(FetchContent)

    FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.8.0
    )
    FetchContent_MakeAvailable(Catch2)

    set(DEBUGCompilerOptions -Wall -Wextra -Wpedantic)
    set(COVCompilerOptions --coverage ${DEBUGCompilerOptions})
    set(COVLinkerOptions --coverage)
    set(SANCompilerOptions -fsanitize=address,undefined ${DEBUGCompilerOptions})
    set(SANLinkerOptions -fsanitize=address,undefined)


    add_executable(BigInt-test BigInt.cxx ../include/BigInt.hpp)
    target_include_directories(BigInt-test PRIVATE ../include)
    target_link_libraries(BigInt-test PRIVATE Catch2::Catch2WithMain)
    if (${CMAKE_BUILD_TYPE} STREQUAL "DebugCOV")
        target_compile_options(BigInt-test PRIVATE
                $<$<CXX_COMPILER_ID:GNU,Clang>:${COVCompilerOptions}>
                $<$<CXX_COMPILER_ID:MSVC>:/W4>
        )
        target_link_options(BigInt-test PRIVATE
                $<$<CXX_COMPILER_ID:GNU,Clang>:${COVLinkerOptions}>
        )
    endif ()
    if (${CMAKE_BUILD_TYPE} STREQUAL "DebugSAN")
        target_compile_options(BigInt-test PRIVATE
                $<$<CXX_COMPILER_ID:GNU,Clang>:${SANCompilerOptions}>
                $<$<CXX_COMPILER_ID:MSVC>:/W4>
        )
        target_link_options(BigInt-test PRIVATE
                $<$<CXX_COMPILER_ID:GNU,Clang>:${SANLinkerOptions}>
        )
    endif ()

    add_test(NAME BigInt-test COMMAND BigInt-test)
    set_tests_properties(BigInt-test PROPERTIES LABELS unit)

    add_executable(BigInt10-test BigInt10.cxx ../include/BigInt10.hpp)
    target_include_directories(BigInt10-test PRIVATE ../include)
    target_link_libraries(BigInt10-test PRIVATE Catch2::Catch2WithMain)
    target_compile_options(BigInt10-test PRIVATE
            $<$<CXX_COMPILER_ID:GNU,Clang>:--coverage -Wall -Wextra -Wpedantic>
            $<$<CXX_COMPILER_ID:MSVC>:/W4>
    )
    target_link_options(BigInt10-test PRIVATE
            $<$<CXX_COMPILER_ID:GNU,Clang>:--coverage -lgcov>
    )

    add_executable(BigInt10-perf-test BigInt10_perf.cxx ../include/BigInt10.hpp)
    target_include_directories(BigInt10-perf-test PRIVATE ../include)
    target_link_libraries(BigInt10-perf-test PRIVATE Catch2::Catch2WithMain)
    target_compile_options(BigInt10-perf-test PRIVATE
            $<$<CXX_COMPILER_ID:GNU,Clang>:--coverage -Wall -Wextra -Wpedantic>
            $<$<CXX_COMPILER_ID:MSVC>:/W4>
    )
    target_link_options(BigInt10-perf-test PRIVATE
            $<$<CXX_COMPILER_ID:GNU,Clang>:--coverage -lgcov>
    )

    add_test(NAME BigInt10-test COMMAND BigInt10-test)
    set_tests_properties(BigInt10-test PROPERTIES LABELS unit)
    add_test(NAME BigInt10-perf-test COMMAND BigInt10-perf-test)
    set_tests_properties(BigInt10-perf-test PROPERTIES LABELS perf)

endif ()
