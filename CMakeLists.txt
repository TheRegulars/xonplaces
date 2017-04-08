cmake_minimum_required(VERSION 3.2)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
project(darkplaces C)
set(SOURCE_ROOT "${PROJECT_SOURCE_DIR}/src")
include(ExternalProject)

# change optimizations for gcc
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
	set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG -fno-strict-aliasing \
	-fno-math-errno -ffinite-math-only -fno-rounding-math  \
	-fno-signaling-nans -fno-trapping-math")
endif()

FUNCTION(PREPEND var prefix)
   SET(listVar "")
   FOREACH(f ${ARGN})
      LIST(APPEND listVar "${prefix}/${f}")
   ENDFOREACH(f)
   SET(${var} "${listVar}" PARENT_SCOPE)
ENDFUNCTION(PREPEND)

ExternalProject_Add(d0_blindid
	GIT_SUBMODULES	  "d0_blind_id"
    SOURCE_DIR        "${CMAKE_SOURCE_DIR}/d0_blind_id/"
	CONFIGURE_COMMAND ${CMAKE_SOURCE_DIR}/d0_blind_id/configure --prefix=<INSTALL_DIR>
	BUILD_COMMAND	  "${MAKE}"
	BUILD_IN_SOURCE   1
	LOG_BUILD         1
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


option(COMPILE_CLIENT        "Build client code" ON)
option(COMPILE_SERVER        "Build dedicated server" ON)
option(COMPILE_MENU          "Enable game menu" ON)
option(COMPILE_CD            "Enable CD player support" ON)
option(COMPILE_VIDEO_CAPTURE "Enable video capture" ON)

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

set(SOURCE_SERVER)
set(SOURCE_CLIENT_COMMON)
set(SOURCE_CLIENT_GLX)
set(SOURCE_CLIENT_SDL)
# TODO: Support definitions for other compilers
set(COMPILE_DEFS "-DLINK_TO_ZLIB -DLINK_TO_LIBJPEG -DLINK_TO_CRYPTO \
				  -DLINK_TO_CRYPTO_RIJNDAEL")
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
find_package(ZLIB REQUIRED)
find_package(JPEG REQUIRED)
# TODO: Use static version of d0_blindid

if (COMPILE_SERVER)
	add_executable(darkplaces-dedicated ${ABS_SOURCE_SERVER} ${ABS_SOURCE_COMMON})
	add_dependencies(darkplaces-dedicated d0_blindid)
	target_compile_definitions(darkplaces-dedicated PRIVATE ${COMPILE_DEFS})
	target_include_directories(darkplaces-dedicated PRIVATE
			${D0_BLINDID_INCLUDE}
		${ZLIB_INCLUDE_DIRS}
		${JPEG_INLUDE_DIR}
	)
    target_link_libraries(darkplaces-dedicated PRIVATE
		${LIBS_COMMON}
		${D0_BLINID_LIBS}
		${ZLIB_LIBRARIES}
		${JPEG_LIBRARIES}
	)
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
			${ZLIB_INCLUDE_DIRS}
			${JPEG_INLUDE_DIR}
			${X11_X11_INCLUDE_DIR}
			${ALSA_INCLUDE_DIR}
		)
		target_link_libraries(darkplaces-glx PRIVATE
			${LIBS_COMMON}
			${D0_BLINID_LIBS}
			${ZLIB_LIBRARIES}
			${JPEG_LIBRARIES}
			${X11_X11_LIB}
			${X11_Xpm_LIB}
			${X11_Xext_LIB}
			${X11_Xxf86vm_LIB}
			${ALSA_LIBRARY}
			Threads::Threads
		)
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
			${ZLIB_INCLUDE_DIRS}
			${JPEG_INLUDE_DIR}
			${SDL2_INCLUDE_DIR}
		)
		target_link_libraries(darkplaces-sdl PRIVATE
			${LIBS_COMMON}
			${D0_BLINID_LIBS}
			${ZLIB_LIBRARIES}
			${ZLIB_LIBRARIES}
			${JPEG_LIBRARIES}
			${SDL2_LIBRARY}
		)
	endif()
endif ()
