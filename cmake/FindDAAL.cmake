
#  DAAL_FOUND        -  system has DAAL
#  DAAL_ROOT_DIR     -  path to the DAAL base directory
#  DAAL_INCLUDE_DIR  -  the DAAL include directory
#  DAAL_LIBRARIES   -  DAAL libraries
#
#
#
# Sample usage:
#
# If DAAL is required (i.e., not an optional part):
#
# ::
#
#    find_package(DAAL REQUIRED)
#    if (DAAL_FOUND)
#        include_directories(${DAAL_INCLUDE_DIR})
#        # and for each of your dependent executable/library targets:
#        target_link_libraries(<YourTarget> ${DAAL_LIBRARIES})
#    endif()

find_path(DAAL_ROOT_DIR NAMES include/daal_sycl.h PATHS $ENV{DAALROOT})

# Convert symlinks to real paths

get_filename_component(DAAL_ROOT_DIR ${DAAL_ROOT_DIR} REALPATH)

if (NOT DAAL_ROOT_DIR)
    if (DAAL_FIND_REQUIRED)
        message(FATAL_ERROR "Could not find DAAL: please set environment variable {DAALROOT}")
    else()
        unset(DAAL_ROOT_DIR CACHE)
    endif()
else()
    set(DAAL_INCLUDE_DIR ${DAAL_ROOT_DIR}/include)

    set(DAAL_LIBRARY_SUFFIX ${CMAKE_SHARED_LIBRARY_SUFFIX})
    set(DAAL_LIBRARY_PREFIX ${CMAKE_SHARED_LIBRARY_PREFIX})
    if (DAAL_USE_static)
        set(DAAL_LIBRARY_SUFFIX ${CMAKE_STATIC_LIBRARY_SUFFIX})
        set(DAAL_LIBRARY_PREFIX ${CMAKE_STATIC_LIBRARY_PREFIX})
    endif()

    if (DAAL_USE_parallel)
        set(DAAL_THREAD_LIBRARY_NAME daal_thread)
        # TODO TBB
    else()
        set(DAAL_THREAD_LIBRARY_NAME daal_sequential)
    endif()

    message(STATUS "daal_core - ${DAAL_LIBRARY_PREFIX}daal_core${DAAL_LIBRARY_SUFFIX}")

    foreach(DAAL_LIB daal_core daal_sycl ${DAAL_THREAD_LIBRARY_NAME})
        message(STATUS "DAAL_LIB ${DAAL_LIB}")
        find_library(
            DAAL_LIBRARY
            NAMES "${DAAL_LIBRARY_PREFIX}${DAAL_LIB}${DAAL_LIBRARY_SUFFIX}"
            PATH_SUFFIXES lib/intel64
            PATHS ${DAAL_ROOT_DIR}
        )

        if (DAAL_LIBRARY)
            list(APPEND DAAL_LIBRARIES ${DAAL_LIBRARY})
        endif()

        unset(DAAL_LIBRARY CACHE)

    endforeach()

    message(STATUS "DAAL_LIBRARIES ${DAAL_LIBRARIES}")

    if (CMAKE_FIND_DEBUG_MODE)
        message(STATUS "Exectuted command: \n${DAAL_LINK_TOOL_COMMAND}")
        message(STATUS "Found DAAL_LIBRARIES:\n${DAAL_LIBRARIES} ")
        message(STATUS "Found DAAL_DEFINITIONS:\n${DAAL_DEFINITIONS} ")
        message(STATUS "Found DAAL_LIBRARY_DIR:\n${DAAL_LIBRARY_DIR} ")
        message(STATUS "Found DAAL_INCLUDE_DIR:\n${DAAL_INCLUDE_DIR} ")
    endif()

endif()