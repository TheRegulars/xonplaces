option(CODE_COVERAGE "Enable code coverage" OFF)
mark_as_advanced(CODE_COVERAGE)

# code coverage
if(CODE_COVERAGE)
    if("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-instr-generate \
            -fcoverage-mapping"
        )
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-instr-generate \
            -fcoverage-mapping"
        )
    elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -fprofile-arcs \
            -ftest-coverage"
        )
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs \
            -ftest-coverage"
        )
    endif()
endif()
