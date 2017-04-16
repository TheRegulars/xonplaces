set(D0_BLINDID_C_FLAGS "$ENV{CFLAGS}" CACHE STRING "C flags for d0_blind_id")
set(D0_BLINDID_EXE_LINKER_FLAGS
    "$ENV{LDFLAGS}" CACHE STRING "linker flags for d0_blind_id")
set(D0_BLINDID_C_FLAGS_RELEASE
    "${D0_BLINDID_C_FLAGS}" CACHE STRING
    "C flags for d0_blind_id Release build"
)
set(D0_BLINDID_EXE_LINKER_FLAGS_RELEASE
    "${D0_BLINDID_EXE_LINKER_FLAGS}" CACHE STRING
    "linker flags for d0_blind_id Release build"
)

if("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    set(D0_BLINDID_C_FLAGS_RELEASE "${D0_BLINDID_C_FLAGS_RELEASE} -O3")
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
    set(D0_BLINDID_C_FLAGS_RELEASE "${D0_BLINDID_C_FLAGS_RELEASE} -O3 -flto")
    set(D0_BLINDID_EXE_LINKER_FLAGS_RELEASE "-flto")
endif()

set(D0_BLINDID_C_FLAGS_NATIVERELEASE
    "${D0_BLINDID_C_FLAGS_RELEASE}" CACHE STRING
    "C flags for d0_blind_id NativeRelease build"
)
set(D0_BLINDID_EXE_LINKER_FLAGS_NATIVERELEASE
    "${D0_BLINDID_EXE_LINKER_FLAGS_RELEASE}" CACHE STRING
    "linker flags for d0_blind_id NativeRelease build"
)

if("${CMAKE_C_COMPILER_ID}" MATCHES "^(GNU|Clang)$")
    set(D0_BLINDID_C_FLAGS_NATIVERELEASE
        "${D0_BLINDID_C_FLAGS_RELEASE} -march=native")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(_D0_BLIND_C_FLAGS "${D0_BLINDID_C_FLAGS}")
    set(_D0_BLIND_EXE_LINKER_FLAGS "${D0_BLINDID_EXE_LINKER_FLAGS}")
else()
    string(TOUPPER "${CMAKE_BUILD_TYPE}" _TEMP_BUILD_TYPE)
    set(_D0_BLIND_C_FLAGS "${D0_BLINDID_C_FLAGS_${_TEMP_BUILD_TYPE}}")
    set(_D0_BLIND_EXE_LINKER_FLAGS
        "${D0_BLINDID_EXE_LINKER_FLAGS_${_TEMP_BUILD_TYPE}}")
endif()


ExternalProject_Add(d0_blindid
    GIT_SUBMODULES    "d0_blind_id"
    SOURCE_DIR        "${CMAKE_SOURCE_DIR}/d0_blind_id/"
    CONFIGURE_COMMAND "${CMAKE_COMMAND}" -E env
                            AR=${CMAKE_AR}
                            CC=${CMAKE_C_COMPILER}
                            LDFLAGS=${_D0_BLIND_EXE_LINKER_FLAGS}
                            CFLAGS=${_D0_BLIND_C_FLAGS}
                      ${CMAKE_SOURCE_DIR}/d0_blind_id/configure --prefix=<INSTALL_DIR>
    BUILD_COMMAND     "${MAKE}"
    BUILD_IN_SOURCE   1
    LOG_BUILD         1
    LOG_CONFIGURE     1
    LOG               1
)

ExternalProject_Add_Step(d0_blindid autoconf
  COMMAND autoreconf -i
  COMMENT "Generate configure and make files with autotools"
  DEPENDERS configure
  DEPENDS "${CMAKE_SOURCE_DIR}/d0_blind_id/configure.ac"
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/d0_blind_id/"
  LOG 1
)

ExternalProject_Get_Property(d0_blindid install_dir)
set(D0_BLINID_LIB_RIJDAEL "${install_dir}/lib/libd0_rijndael.so")
set(D0_BLINID_ID_LIB "${install_dir}/lib/libd0_blind_id.so")
set(D0_BLINID_LIBS ${D0_BLINID_LIB_RIJDAEL} ${D0_BLINID_ID_LIB})
set(D0_BLINID_LIB_RIJDAEL_STATIC "${install_dir}/lib/libd0_rijndael.a")
set(D0_BLINID_ID_LIB_STATIC "${install_dir}/lib/libd0_blind_id.a")
set(D0_BLINID_LIBS_STATIC ${D0_BLINID_LIB_RIJDAEL_STATIC} ${D0_BLINID_ID_LIB_STATIC})
set(D0_BLINDID_INCLUDE "${CMAKE_SOURCE_DIR}")
file(GLOB D0_BLINDID_INSTALL_LIBS "${install_dir}/lib/lib*.so*")

mark_as_advanced(
    D0_BLINDID_C_FLAGS
    D0_BLINDID_EXE_LINKER_FLAGS
    D0_BLINDID_C_FLAGS_RELEASE
    D0_BLINDID_EXE_LINKER_FLAGS_RELEASE
    D0_BLINDID_C_FLAGS_NATIVERELEASE
    D0_BLINDID_EXE_LINKER_FLAGS_NATIVERELEASE
)
