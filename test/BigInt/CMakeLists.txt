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

    add_library(
            common-options
            INTERFACE
    )
    target_compile_options(
            common-options
            INTERFACE
            $<$<CONFIG:Debug>:$<$<CXX_COMPILER_ID:GNU,Clang>:${DEBUGCompilerOptions}>>
            $<$<CONFIG:Coverage>:$<$<CXX_COMPILER_ID:GNU,Clang>:${COVCompilerOptions}>>
            $<$<CONFIG:Sanitizer>:$<$<CXX_COMPILER_ID:GNU,Clang>:${SANCompilerOptions}>>
            $<$<CXX_COMPILER_ID:MSVC>:/W4>
    )
    target_link_options(
            common-options
            INTERFACE
            $<$<CONFIG:Coverage>:$<$<CXX_COMPILER_ID:GNU,Clang>:${COVLinkerOptions}>>
            $<$<CONFIG:Sanitizer>:$<$<CXX_COMPILER_ID:GNU,Clang>:${SANLinkerOptions}>>
    )

    add_executable(BigInt-core)
    target_sources(
            BigInt-core
            PRIVATE
            BigInt-core.cxx
    )
    target_include_directories(
            BigInt-core
            PRIVATE
            ../../include
    )
    target_link_libraries(
            BigInt-core
            PRIVATE
            common-options
            Catch2::Catch2WithMain
    )

    add_executable(templated-operators)
    target_sources(
            templated-operators
            PRIVATE
            templated-operators.cxx
    )
    target_include_directories(
            templated-operators
            PRIVATE
            ../../include
    )
    target_link_libraries(
            templated-operators
            PRIVATE
            common-options
            Catch2::Catch2WithMain
    )

    add_test(NAME BigInt-core COMMAND BigInt-core)
    set_tests_properties(BigInt-core PROPERTIES LABELS unit)
    add_test(NAME templated-operators COMMAND templated-operators)
    set_tests_properties(templated-operators PROPERTIES LABELS unit)

endif ()
