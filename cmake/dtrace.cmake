find_program(DTRACE_EXECUTABLE NAMES dtrace stap-dtrace)
mark_as_advanced(DTRACE_EXECUTABLE)
find_package(Perl)

option(ENABLE_DTRACE "Enable USDT probes (Dtrace/Trace events" ON)

set(USDT_OBJECT_FILE ${CMAKE_BINARY_DIR}/probes_usdt.o)
set(PROBES_HEADER ${CMAKE_BINARY_DIR}/include/tracing_probes.h)

MACRO(DTRACE_HEADER provider header)
    if(ENABLE_DTRACE)
        ADD_CUSTOM_COMMAND(
            OUTPUT ${header}
            COMMAND ${DTRACE_EXECUTABLE} -h -s ${provider} -o ${header}
            DEPENDS ${provider}
        )
    else()
        ADD_CUSTOM_COMMAND(
            OUTPUT ${header}
            COMMAND ${PERL_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/dheadgen.pl -f ${provider} > ${header}
            DEPENDS ${provider}
        )
    endif()
ENDMACRO()

DTRACE_HEADER(${CMAKE_SOURCE_DIR}/tracing.d ${PROBES_HEADER})
ADD_CUSTOM_TARGET(
    gen_dtrace_headers
    DEPENDS
    ${CMAKE_SOURCE_DIR}/tracing.d
    ${PROBES_HEADER}
)
if(ENABLE_DTRACE)
    ADD_CUSTOM_COMMAND(
        OUTPUT ${USDT_OBJECT_FILE}
        COMMAND ${DTRACE_EXECUTABLE} -G -s ${CMAKE_SOURCE_DIR}/tracing.d -o ${USDT_OBJECT_FILE}
        DEPENDS ${CMAKE_SOURCE_DIR}/tracing.d
    )
    ADD_CUSTOM_TARGET(
        gen_dtrace_objects
        DEPENDS
        ${CMAKE_SOURCE_DIR}/tracing.d
        ${USDT_OBJECT_FILE}
    )
endif()

MACRO(ADD_DTRACE target)
    ADD_DEPENDENCIES(${target} gen_dtrace_headers)
    if(ENABLE_DTRACE)
        ADD_DEPENDENCIES(${target} gen_dtrace_objects)
    endif()
ENDMACRO()
