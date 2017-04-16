cmake_minimum_required(VERSION 3.2)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
project(darkplaces C)
set(SOURCE_ROOT "${PROJECT_SOURCE_DIR}/src")
include(ExternalProject)

if("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pipe")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -O3 -DNDEBUG
    -fno-strict-aliasing -fno-math-errno -ffinite-math-only -fno-rounding-math \
    -fno-signaling-nans -fno-trapping-math")
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
    # Use LTO when building using release with clang
    # NOTE: This requires llvm-ar and gold plugin
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -O3 -DNDEBUG
    -fno-strict-aliasing -fno-math-errno -ffinite-math-only -fno-trapping-math \
    -flto")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-flto")
endif()

# Add NativeRelease build type
set(CMAKE_C_FLAGS_NATIVERELEASE
    "${CMAKE_C_FLAGS_RELEASE}" CACHE STRING "C flags for native build")
set(CMAKE_EXE_LINKER_FLAGS_NATIVERELEASE
    "${CMAKE_EXE_LINKER_FLAGS_RELEASE}" CACHE STRING "linker flags for native build")
mark_as_advanced(
    CMAKE_C_FLAGS_NATIVERELEASE
    CMAKE_EXE_LINKER_FLAGS_NATIVERELEASE
)

if("${CMAKE_C_COMPILER_ID}" MATCHES "^(GNU|Clang)$")
    set(CMAKE_C_FLAGS_NATIVERELEASE
        "${CMAKE_C_FLAGS_NATIVERELEASE} -march=native")
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        "Debug" "Release" "NativeRelease" "MinSizeRel" "RelWithDebInfo")
endif()

FUNCTION(PREPEND var prefix)
   SET(listVar "")
   FOREACH(f ${ARGN})
      LIST(APPEND listVar "${prefix}/${f}")
   ENDFOREACH(f)
   SET(${var} "${listVar}" PARENT_SCOPE)
ENDFUNCTION(PREPEND)

option(COMPILE_CLIENT        "Build client code" ON)
option(COMPILE_SERVER        "Build dedicated server" ON)
option(COMPILE_MENU          "Enable game menu" ON)
option(COMPILE_CD            "Enable CD player support" ON)
option(COMPILE_VIDEO_CAPTURE "Enable video capture" ON)
option(COMPILE_UTILS         "Enable compilling of various utils" OFF)
option(STATIC_LINK_D0        "Link statically with d0 blind id" OFF)
option(CPACK_PACKAGING       "Build package with CPACK" OFF)

# Global build params
SET(CMAKE_INSTALL_RPATH "$ORIGIN/../lib:$ORIGIN/lib:$ORIGIN/")

# detect client type
SET(DP_CLIENT_GLX OFF)
SET(DP_CLIENT_SDL OFF)
set(DP_CLIENT_AGL OFF)
set(DP_CLIENT_WGL OFF)
if(COMPILE_CLIENT)
    if(UNIX)
        SET(DP_CLIENT_GLX ON)
        SET(DP_CLIENT_SDL ON)
    endif()
    # TODO: Implement other video backends
endif()

set(SOURCE_COMMON
    builddate.c
    sys.h
    bih.c bih.h
    crypto.c crypto.h
    cl_collision.c cl_collision.h
    cl_demo.c
    cl_dyntexture.c cl_dyntexture.h
    cl_input.c
    cl_main.c
    cl_parse.c
    cl_particles.c
    cl_screen.c cl_screen.h
    cl_video.c cl_video.h
    clvm_cmds.c clvm_cmds.h
    cmd.c cmd.h
    collision.c collision.h
    common.c common.h
    console.c console.h
    csprogs.c csprogs.h
    curves.c curves.h
    cvar.c cvar.h
    dpsoftrast.c dpsoftrast.h
    dpvsimpledecode.c dpvsimpledecode.h
    filematch.c
    fractalnoise.c
    fs.c fs.h
    ft2.c ft2.h ft2_defs.h ft2_fontdefs.h
    utf8lib.c utf8lib.h
    gl_backend.c gl_backend.h
    gl_draw.c lhfont.h
    gl_rmain.c
    gl_rsurf.c
    gl_textures.c
    hmac.c hmac.h
    host.c
    host_cmd.c
    image.c image.h
    image_png.c image_png.h
    jpeg.c jpeg.h
    keys.c keys.h
    lhnet.c lhnet.h
    libcurl.c libcurl.h
    mathlib.c mathlib.h
    matrixlib.c matrixlib.h
    mdfour.c mdfour.h
    meshqueue.c meshqueue.h
    mod_skeletal_animatevertices_sse.c mod_skeletal_animatevertices_sse.h
    mod_skeletal_animatevertices_generic.c mod_skeletal_animatevertices_generic.h
    model_alias.c model_alias.h
    model_brush.c model_brush.h
    model_shared.c model_shared.h
    model_sprite.c model_sprite.h
    netconn.c netconn.h
    palette.c palette.h
    polygon.c polygon.h
    portals.c portals.h
    protocol.c protocol.h
    prvm_cmds.c prvm_cmds.h
    prvm_edict.c
    prvm_exec.c
    r_explosion.c
    r_lerpanim.c r_lerpanim.h
    r_lightning.c
    r_modules.c r_modules.h
    r_shadow.c r_shadow.h
    r_sky.c
    r_sprites.c
    sbar.c sbar.h
    sv_demo.c sv_demo.h
    sv_main.c
    sv_move.c
    sv_phys.c
    sv_user.c
    svbsp.c svbsp.h
    svvm_cmds.c
    sys_shared.c
    vid_shared.c vid.h
    view.c
    wad.c wad.h
    world.c world.h
    zone.c zone.h
    )


set(CRYPTO_KEYGEN_SOURCE ${SOURCE_ROOT}/utils/crypto-keygen-standalone.c)

set(SOURCE_SERVER)
set(SOURCE_CLIENT_COMMON)
set(SOURCE_CLIENT_GLX)
set(SOURCE_CLIENT_SDL)
# TODO: Support definitions for other compilers
set(COMPILE_DEFS "-DLINK_TO_CRYPTO -DLINK_TO_CRYPTO_RIJNDAEL")
set(COMPILE_DEFS_CLIENT)

if (COMPILE_MENU)
    list(APPEND COMPILE_DEFS_CLIENT "-DCONFIG_MENU")
    list(APPEND SOURCE_CLIENT_COMMON menu.c menu.h mprogdefs.h mvm_cmds.c)
endif ()

if (COMPILE_CD)
    list(APPEND COMPILE_DEFS_CLIENT "-DCONFIG_CD")
    list(APPEND SOURCE_CLIENT_COMMON cd_shared.c cdaudio.h)
    list(APPEND SOURCE_CLIENT_SDL cd_sdl.c)
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        list(APPEND SOURCE_CLIENT_GLX cd_linux.c)
    elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
        list(APPEND SOURCE_CLIENT_GLX cd_bsd.c)
    endif()
endif ()

if (COMPILE_VIDEO_CAPTURE)
    list(APPEND COMPILE_DEFS_CLIENT -DCONFIG_VIDEO_CAPTURE)
    list(APPEND SOURCE_CLIENT_COMMON cap_avi.c cap_avi.h cap_ogg.c cap_ogg.h)
endif ()

list(APPEND SOURCE_CLIENT_SDL vid_sdl.c)
list(APPEND SOURCE_CLIENT_GLX vid_glx.c)
list(APPEND SOURCE_SERVER vid_null.c)

list(APPEND SOURCE_CLIENT_COMMON
        snd_main.c snd_main.h snd_mem.c snd_mix.c snd_ogg.c snd_ogg.h snd_wav.c
        snd_wav.h)
list(APPEND SOURCE_CLIENT_SDL snd_sdl.c sys_sdl.c)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND SOURCE_CLIENT_GLX snd_alsa.c sys_linux.c)
elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    list(APPEND SOURCE_CLIENT_GLX snd_bsd.c sys_linux.c)
endif()
list(APPEND SOURCE_SERVER snd_null.c)

if (WIN32)
    list(APPEND SOURCE_SERVER sys_win.c)
    list(APPEND SOURCE_COMMON conproc.c conproc.h)
else ()
    list(APPEND SOURCE_SERVER sys_linux.c)
endif ()

list(APPEND SOURCE_COMMON thread.h)
list(APPEND SOURCE_CLIENT_SDL thread_sdl.c)
list(APPEND SOURCE_CLIENT_GLX thread_pthread.c)
list(APPEND SOURCE_SERVER thread_null.c)

list(APPEND SOURCE_CLIENT_GLX keysym2ucs.c)


set(LIBS_COMMON ${CMAKE_DL_LIBS})
if (UNIX)
    list(APPEND LIBS_COMMON m)
endif ()
if (WIN32)
    list(APPEND LIBS_COMMON ws2_32 winmm)
endif ()
PREPEND(ABS_SOURCE_COMMON ${SOURCE_ROOT} ${SOURCE_COMMON})
PREPEND(ABS_SOURCE_SERVER ${SOURCE_ROOT} ${SOURCE_SERVER})
PREPEND(ABS_SOURCE_CLIENT_COMMON ${SOURCE_ROOT} ${SOURCE_CLIENT_COMMON})
PREPEND(ABS_SOURCE_CLIENT_SDL ${SOURCE_ROOT} ${SOURCE_CLIENT_SDL})
PREPEND(ABS_SOURCE_CLIENT_GLX ${SOURCE_ROOT} ${SOURCE_CLIENT_GLX})

# load common libraries
find_package(CURL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(JPEG REQUIRED)
# add d0_blind_id
include(D0BlindId)

if (STATIC_LINK_D0)
    # TODO: Make separate script: FindGMP
    set(GMP_PREFIX "" CACHE PATH "Path to GMP prefix")
    find_library(GMP_LIBRARY NAMES gmp PATHS
        ${GMP_PREFIX}/lib
        /usr/lib
        /usr/local/lib
        /opt/lib
        /opt/local/lib
    )
    mark_as_advanced(GMP_PREFIX GMP_LIBRARY)
    set(D0_LIBS ${D0_BLINID_LIBS_STATIC} ${GMP_LIBRARY})
else()
    set(D0_LIBS ${D0_BLINID_LIBS})
    install(FILES ${D0_BLINDID_INSTALL_LIBS} DESTINATION lib)
endif()

if (COMPILE_SERVER)
    add_executable(darkplaces-dedicated ${ABS_SOURCE_SERVER} ${ABS_SOURCE_COMMON})
    add_dependencies(darkplaces-dedicated d0_blindid)
    target_compile_definitions(darkplaces-dedicated PRIVATE ${COMPILE_DEFS})
    target_include_directories(darkplaces-dedicated PRIVATE
        ${D0_BLINDID_INCLUDE}
        ${CURL_INCLUDE_DIRS}
        ${ZLIB_INCLUDE_DIRS}
        ${JPEG_INLUDE_DIR}
    )
    target_link_libraries(darkplaces-dedicated PRIVATE
        ${LIBS_COMMON}
        ${D0_LIBS}
        ${CURL_LIBRARIES}
        ${ZLIB_LIBRARIES}
        ${JPEG_LIBRARIES}
    )
    install(TARGETS darkplaces-dedicated RUNTIME DESTINATION bin)
endif ()

if (COMPILE_CLIENT)
    if (DP_CLIENT_GLX)
        find_package(Threads REQUIRED)
        # TODO: Support for FreeBSD
        find_package(ALSA REQUIRED)
        set(CMAKE_THREAD_PREFER_PTHREAD ON)
        find_package(X11 REQUIRED)
        add_executable(darkplaces-glx
            ${ABS_SOURCE_COMMON}
            ${ABS_SOURCE_CLIENT_COMMON}
            ${ABS_SOURCE_CLIENT_GLX}
        )
        add_dependencies(darkplaces-glx d0_blindid)
        target_compile_definitions(darkplaces-glx PRIVATE
            ${COMPILE_DEFS}
            ${COMPILE_DEFS_CLIENT}
        )
        target_include_directories(darkplaces-glx PRIVATE
            ${D0_BLINDID_INCLUDE}
            ${CURL_INCLUDE_DIRS}
            ${ZLIB_INCLUDE_DIRS}
            ${JPEG_INLUDE_DIR}
            ${X11_X11_INCLUDE_DIR}
            ${ALSA_INCLUDE_DIR}
        )
        target_link_libraries(darkplaces-glx PRIVATE
            ${LIBS_COMMON}
            ${D0_LIBS}
            ${CURL_LIBRARIES}
            ${ZLIB_LIBRARIES}
            ${JPEG_LIBRARIES}
            ${X11_X11_LIB}
            ${X11_Xpm_LIB}
            ${X11_Xext_LIB}
            ${X11_Xxf86vm_LIB}
            ${ALSA_LIBRARY}
            Threads::Threads
        )
        install(TARGETS darkplaces-glx RUNTIME DESTINATION bin)
    endif()
    if (DP_CLIENT_SDL)
        find_package(SDL2 REQUIRED)
        add_executable(darkplaces-sdl
            ${ABS_SOURCE_COMMON}
            ${ABS_SOURCE_CLIENT_COMMON}
            ${ABS_SOURCE_CLIENT_SDL}
        )
        add_dependencies(darkplaces-sdl d0_blindid)
        target_compile_definitions(darkplaces-sdl PRIVATE
            ${COMPILE_DEFS}
            ${COMPILE_DEFS_CLIENT}
        )
        target_include_directories(darkplaces-sdl PRIVATE
            ${D0_BLINDID_INCLUDE}
            ${CURL_INCLUDE_DIRS}
            ${ZLIB_INCLUDE_DIRS}
            ${JPEG_INLUDE_DIR}
            ${SDL2_INCLUDE_DIR}
        )
        target_link_libraries(darkplaces-sdl PRIVATE
            ${LIBS_COMMON}
            ${D0_LIBS}
            ${CURL_LIBRARIES}
            ${ZLIB_LIBRARIES}
            ${ZLIB_LIBRARIES}
            ${JPEG_LIBRARIES}
            ${SDL2_LIBRARY}
        )
        install(TARGETS darkplaces-sdl RUNTIME DESTINATION bin)
    endif()
endif ()

if(COMPILE_UTILS)
    add_executable(crypto-keygen ${CRYPTO_KEYGEN_SOURCE})
    add_dependencies(crypto-keygen d0_blindid)
    target_include_directories(crypto-keygen PRIVATE ${D0_BLINDID_INCLUDE})
    target_link_libraries(crypto-keygen PRIVATE ${LIBS_COMMON} ${D0_LIBS})
    install(TARGETS crypto-keygen RUNTIME DESTINATION bin)
endif()

if(CPACK_PACKAGING)
    string(TIMESTAMP PACKAGE_DATE "%Y-%m-%d" UTC)
    set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
    set(CPACK_PACKAGE_VERSION_MAJOR "0")
    set(CPACK_PACKAGE_VERSION_MINOR "1")
    set(CPACK_PACKAGE_VERSION_PATCH "1")
    set(CPACK_PACKAGE_VERSION
        "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}"
    )
    set(CPACK_PACKAGE_FILE_NAME
        "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${PACKAGE_DATE}"
    )
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Slava Bacherikov")
    set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Darkplaces game engine")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS
        "libc6 (>= 2.11), zlib1g (>= 1:1.1.4), libcurl3-gnutls (>= 7.16.2)"
    )
    if(DP_CLIENT_SDL)
        set(CPACK_DEBIAN_PACKAGE_DEPENDS
            "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libsdl2-2.0-0 (>= 2.0.4)"
        )
    endif()
    # TODO: set generators depending on platform
    set(CPACK_GENERATOR "TGZ" "ZIP" "DEB" "RPM")
    include(CPack)
endif()
