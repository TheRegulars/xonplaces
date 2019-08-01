
if("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
    option(SANITIZERS_ADDRESS_ENABLE "Enable Address Sanitizer" OFF)
    option(SANITIZERS_MEMORY_ENABLE "Enable Memory Sanitizer" OFF)
    option(SANITIZERS_HWASAN_ENABLE "Enable Hardware-assisted Address Sanitizer" OFF)
    option(SANITIZERS_UNDEFINED_ENABLE "Enable UB Sanitizer" OFF)

    # TODO: add ControlFlowIntegrity sanitizer
    mark_as_advanced(
        SANITIZERS_ADDRESS_ENABLE,
        SANITIZERS_MEMORY_ENABLE,
        SANITIZERS_HWASAN_ENABLE,
        SANITIZERS_UNDEFINED_ENABLE
    )

    set(SANITIZERS_LIST CACHE INTERNAL FORCE)
    #set(SANITIZERS_CLI_OPTION CACHE INTERNAL)

    if(SANITIZERS_ADDRESS_ENABLE)
        list(APPEND SANITIZERS_LIST "address")
    endif()
    if(SANITIZERS_MEMORY_ENABLE)
        list(APPEND SANITIZERS_LIST "memory")
    endif()
    if(SANITIZERS_UNDEFINED_ENABLE)
        list(APPEND SANITIZERS_LIST "undefined")
    endif()
    if(SANITIZERS_HWASAN_ENABLE)
        message("Hardware Asisted Address sanitizer works only on AArch64")
        list(APPEND SANITIZERS_LIST "hwaddress")
    endif()

    if((SANITIZERS_ADDRESS_ENABLE OR SANITIZERS_HWASAN_ENABLE) AND SANITIZERS_MEMORY_ENABLE)
        message(FATAL_ERROR "Address and Memory santizer can't be enabled in same time")
    endif()
    if(SANITIZERS_ADDRESS_ENABLE AND SANITIZERS_HWASAN_ENABLE)
        message(FATAL_ERROR "Address and Hardware Adress santizer can't be enabled in same time")
    endif()

    if(SANITIZERS_ADDRESS_ENABLE OR
       SANITIZERS_MEMORY_ENABLE OR
       SANITIZERS_HWASAN_ENABLE OR
       SANITIZERS_UNDEFINED_ENABLE
    )
        list(JOIN SANITIZERS_LIST "," SANITIZERS_CLI_OPTION)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-omit-frame-pointer -fsanitize=${SANITIZERS_CLI_OPTION}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -fsanitize=${SANITIZERS_CLI_OPTION}")
    endif()


endif()
