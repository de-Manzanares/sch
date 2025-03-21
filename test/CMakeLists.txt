if (SCH_ENABLE_TESTS)

    include(CTest)
    include(FetchContent)

    FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.8.0
    )
    FetchContent_MakeAvailable(Catch2)

    set(CompilerOptions -Wall -Wextra -Wpedantic)
    set(DEBUGCompilerOptions -g ${CompilerOptions})
    set(COVCompilerOptions --coverage ${CompilerOptions})
    set(COVLinkerOptions --coverage)
    set(SANCompilerOptions -fsanitize=address,undefined ${CompilerOptions})
    set(SANLinkerOptions -fsanitize=address,undefined)

    add_executable(BigInt-test BigInt.cxx ../include/BigInt.hpp)
    target_include_directories(BigInt-test PRIVATE ../include)
    target_link_libraries(BigInt-test PRIVATE Catch2::Catch2WithMain)
    if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        target_compile_options(BigInt-test PRIVATE
                $<$<CXX_COMPILER_ID:GNU,Clang>:${DEBUGCompilerOptions}>
                $<$<CXX_COMPILER_ID:MSVC>:/W4>
        )
    elseif (${CMAKE_BUILD_TYPE} STREQUAL "Coverage")
        target_compile_options(BigInt-test PRIVATE
                $<$<CXX_COMPILER_ID:GNU,Clang>:${COVCompilerOptions}>
                $<$<CXX_COMPILER_ID:MSVC>:/W4>
        )
        target_link_options(BigInt-test PRIVATE
                $<$<CXX_COMPILER_ID:GNU,Clang>:${COVLinkerOptions}>
        )
    elseif (${CMAKE_BUILD_TYPE} STREQUAL "Sanitizer")
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

    add_executable(BigInt-perf-test BigInt_perf.cxx ../include/BigInt.hpp)
    target_include_directories(BigInt-perf-test PRIVATE ../include)
    target_link_libraries(BigInt-perf-test PRIVATE Catch2::Catch2WithMain)
    target_compile_options(BigInt-perf-test PRIVATE
            $<$<CXX_COMPILER_ID:GNU,Clang>:--coverage -Wall -Wextra -Wpedantic>
            $<$<CXX_COMPILER_ID:MSVC>:/W4>
    )
    target_link_options(BigInt-perf-test PRIVATE
            $<$<CXX_COMPILER_ID:GNU,Clang>:--coverage -lgcov>
    )

    add_test(NAME BigInt-perf-test COMMAND BigInt-perf-test)
    set_tests_properties(BigInt-perf-test PROPERTIES LABELS perf)

endif ()
