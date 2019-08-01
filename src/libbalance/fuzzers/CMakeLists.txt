
if("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang" AND BUILD_FUZZERS)
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../")
    add_executable(fuzz_teambalance teambalance_fuzzer.cpp "../balance.h")
    set_target_properties(fuzz_teambalance PROPERTIES LINKER_LANGUAGE CXX)
    target_link_libraries(fuzz_teambalance PUBLIC balance)
    string(JOIN "," FUZZERS_CLI "${SANITIZERS_CLI_OPTION}" "fuzzer")
    target_compile_options(fuzz_teambalance PRIVATE -fno-omit-frame-pointer
        -g -fsanitize=${FUZZERS_CLI} -fsanitize-coverage=trace-cmp
    )
    target_link_options(fuzz_teambalance PRIVATE -fsanitize=${FUZZERS_CLI}
        -fsanitize-coverage=trace-cmp)
    add_custom_target(fuzz_libbalance
        COMMAND fuzz_teambalance "${CMAKE_CURRENT_SOURCE_DIR}/corpus/"
                "-dict=${CMAKE_CURRENT_SOURCE_DIR}/balance_dict.txt"
                "-max_len=110"
        DEPENDS fuzz_teambalance
    )
endif()
