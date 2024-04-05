cmake_minimum_required(VERSION 3.12)
#------------------------------------------------------------------------------
# Version information
#------------------------------------------------------------------------------

set(HPC_AIO_VERSION_MAJOR "0")
set(HPC_AIO_VERSION_MINOR "1")
set(HPC_AIO_VERSION_PATCH "0")
set(HPC_AIO_PACKAGE ${PROJECT_NAME})
set(HPC_AIO_PACKAGE_NAME ${PROJECT_NAME})
set(HPC_AIO_PACKAGE_VERSION "${HPC_AIO_VERSION_MAJOR}.${HPC_AIO_VERSION_MINOR}.${HPC_AIO_VERSION_PATCH}")
set(HPC_AIO_PACKAGE_VERSION_MAJOR "${HPC_AIO_VERSION_MAJOR}.${HPC_AIO_VERSION_MINOR}")
set(HPC_AIO_PACKAGE_VERSION_MINOR "${HPC_AIO_VERSION_PATCH}")
set(HPC_AIO_PACKAGE_STRING "${HPC_AIO_PACKAGE_NAME} ${HPC_AIO_PACKAGE_VERSION}")
set(HPC_AIO_PACKAGE_TARNAME "${HPC_AIO_PACKAGE}")

project(hpc_aio LANGUAGES C CXX)

# Convenience defines
string(TOUPPER "${PROJECT_NAME}" UPPER_PROJECT_NAME)
string(TOLOWER "${PROJECT_NAME}" LOWER_PROJECT_NAME)

#------------------------------------------------------------------------------
# Internal Includes for header and libraries
#------------------------------------------------------------------------------
# Get installation directories -- these get used in various places;
# best to just make them available
option (HPC_AIO_LIBDIR_AS_LIB OFF)
if (NOT HPC_AIO_LIBDIR_AS_LIB)
    include(GNUInstallDirs)
endif ()
#
if (CMAKE_INSTALL_LIBDIR)
   message(STATUS "CMAKE_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR}")
   set(HPC_AIO_LIBDIR ${CMAKE_INSTALL_LIBDIR})
   set(HPC_AIO_INSTALL_INCLUDE_DIR
       ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR})
   set(HPC_AIO_INSTALL_DOCDIR
       ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DOCDIR})
   set(HPC_AIO_INSTALL_SYSCONFDIR
       ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_SYSCONFDIR}/modulefiles)
else ()
   set(HPC_AIO_LIBDIR "lib")
   set(HPC_AIO_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include")
   set(HPC_AIO_INSTALL_DOCDIR "${CMAKE_INSTALL_PREFIX}/doc")
   set(HPC_AIO_INSTALL_SYSCONFDIR "${CMAKE_INSTALL_PREFIX}/etc/modulefiles")
   message(STATUS "HPC_AIO_LIBDIR set to ${HPC_AIO_LIBDIR}")
endif ()

#------------------------------------------------------------------------------
# Internal Paths for cmake libraries and Setup install and output Directories
#------------------------------------------------------------------------------
# This sets where to look for dependent libraries
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${CMAKE_BINARY_DIR} ${CMAKE_INSTALL_PREFIX})
# This sets where to look for dependent library's cmake files
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR}/${HPC_AIO_LIBDIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR}/share/cmake)

#------------------------------------------------------------------------------
if (NOT HPC_AIO_EXTERNALLY_CONFIGURED)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin CACHE PATH "Single Directory for all Executables.")
    set(CMAKE_INCLUDE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/include CACHE PATH "Store the headers.")
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${HPC_AIO_LIBDIR} CACHE PATH "Single Directory for all Libraries")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${HPC_AIO_LIBDIR} CACHE PATH "Single Directory for all static libraries.")
endif ()

#-----------------------------------------------------------------------------
# Targets built within this project are exported at Install time for use
# by other projects.
#-----------------------------------------------------------------------------
if (NOT HPC_AIO_EXPORTED_TARGETS)
    set(HPC_AIO_EXPORTED_TARGETS "hpc_aio-targets")
endif ()
#------------------------------------------------------------------------------
# Build options
#------------------------------------------------------------------------------
# option(ENABLE_HPC_AIO_DEBUG "Include debugging prints and logging" OFF) # This is not needed as we have CMAKE_BUILD_TYPE
# This is verbose, maybe an alternate might help simplify
#option(BUILD_URPC "Build HPC_AIO's URPC code" OFF)
#option(ENABLE_PERFFLOW "Build with PerfFlow Aspect support" OFF)
#option(ENABLE_UCX_DTL "Build HPC_AIO's UCX data transport layer" OFF)

# This option is off by default because non-developers should not use
# this option under normal circumstances.
option(HPC_AIO_WARNINGS_AS_ERRORS "Build with warnings promoted to errors." OFF)
mark_as_advanced(HPC_AIO_WARNINGS_AS_ERRORS)

option (HPC_AIO_USE_CLANG_LIBCXX OFF)
if (HPC_AIO_USE_CLANG_LIBCXX)
    set (GCC_TOOLCHAIN "")
endif (HPC_AIO_USE_CLANG_LIBCXX)

# Public
set(HPC_AIO_COMMUNICATION "UCX" CACHE STRING "Communications to use for HPC_AIO")
set_property(CACHE HPC_AIO_COMMUNICATION PROPERTY STRINGS UCX MPI)
set(HPC_AIO_RUNTIME "LIBUV" CACHE STRING "Event driven runtime to use for HPC_AIO")
set_property(CACHE HPC_AIO_COMMUNICATION PROPERTY STRINGS LIBUV)

# Profiling and logging
set(HPC_AIO_PROFILER "NONE" CACHE STRING "Profiler to use for HPC_AIO")
set_property(CACHE HPC_AIO_PROFILER PROPERTY STRINGS DLIO_PROFILER NONE)
set(HPC_AIO_LOGGER "NONE" CACHE STRING "Logger to use for HPC_AIO")
set_property(CACHE HPC_AIO_LOGGER PROPERTY STRINGS CPP_LOGGER NONE)
set(HPC_AIO_LOGGER_LEVEL "NONE" CACHE STRING "Logging level to use for HPC_AIO")
set_property(CACHE HPC_AIO_LOGGER_LEVEL PROPERTY STRINGS DEBUG INFO WARN ERROR NONE)

# Testing
option (HPC_AIO_ENABLE_TESTING "Enable tests for HPC_AIO." OFF)
option (HPC_AIO_ENABLE_POC_TESTING "Enable POC tests for HPC_AIO." OFF)


#------------------------------------------------------------------------------
# Compiler setup
#------------------------------------------------------------------------------
include(ExternalProject)
# It requires include(GNUInstallDirs) before if that is needed
include(SetupCompiler)

#------------------------------------------------------------------------------
# Build Type based configurations
#------------------------------------------------------------------------------

if(CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release") # Setting default as Release
endif()

if(HPC_AIO_PROFILER STREQUAL "DLIO_PROFILER")
    set(HPC_AIO_PROFILER_DLIO_PROFILER 1)
else()
    set(HPC_AIO_PROFILER_NONE 1)
endif()

if(HPC_AIO_LOGGER STREQUAL "CPP_LOGGER")
    set(HPC_AIO_LOGGER_CPP_LOGGER 1)
endif()


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (HPC_AIO_LOGGER STREQUAL "NONE")
        message(STATUS "[${PROJECT_NAME}] disabling logger as no logger is selected")
        set(HPC_AIO_LOGGER_NO_LOG 1)
    else()
        if (HPC_AIO_LOGGER_LEVEL STREQUAL "NONE" OR HPC_AIO_LOGGER_LEVEL STREQUAL "DEBUG")
            set(HPC_AIO_LOGGER_LEVEL_ERROR 1)
            set(HPC_AIO_LOGGER_LEVEL_WARN 1)
            set(HPC_AIO_LOGGER_LEVEL_INFO 1)
            set(HPC_AIO_LOGGER_LEVEL_DEBUG 1)
        elseif(HPC_AIO_LOGGER_LEVEL STREQUAL "ERROR")
            set(HPC_AIO_LOGGER_LEVEL_ERROR 1)
        elseif(HPC_AIO_LOGGER_LEVEL STREQUAL "WARN")
            set(HPC_AIO_LOGGER_LEVEL_ERROR 1)
            set(HPC_AIO_LOGGER_LEVEL_WARN 1)
        elseif(HPC_AIO_LOGGER_LEVEL STREQUAL "INFO")
            set(HPC_AIO_LOGGER_LEVEL_ERROR 1)
            set(HPC_AIO_LOGGER_LEVEL_WARN 1)
            set(HPC_AIO_LOGGER_LEVEL_INFO 1)
        else()
            set(HPC_AIO_LOGGER_NO_LOG 1)
        endif()
    endif()
else()
    set(HPC_AIO_LOGGER_NO_LOG 1)
endif()

if (NOT DEFINED BUILD_SHARED_LIBS)
    set(BUILD_SHARED_LIBS ON)
endif ()
#------------------------------------------------------------------------------
# Disallow in-source build
#------------------------------------------------------------------------------
if ("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR
            "${PROJECT_NAME} requires an out of source Build. "
            "Please create a separate binary directory and run CMake there.")
endif ()
#------------------------------------------------------------------------------
# Set a default build type if none was specified
#------------------------------------------------------------------------------
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
            "MinSizeRel" "RelWithDebInfo")
endif ()
#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------

# Required Dependencies
# =============================================================================
if(HPC_AIO_COMMUNICATION STREQUAL "UCX")
    set(HPC_AIO_COMMUNICATION_ENABLE_UCX 1)
    find_package(ucx 1.6 REQUIRED)
    if (${ucx_FOUND})
        message(STATUS "[${PROJECT_NAME}] found ucx at ${ucx_INCLUDE_DIRS}")
    else ()
        message(FATAL_ERROR "-- [${PROJECT_NAME}] ucx is needed for ${PROJECT_NAME} build")
    endif ()
endif()

if(HPC_AIO_RUNTIME STREQUAL "LIBUV")
    set(HPC_AIO_RUNTIME_ENABLE_LIBUV 1)
    find_package(libuv REQUIRED)
    if (${ucx_FOUND})
        message(STATUS "[${PROJECT_NAME}] found libuv at ${LIBUV_INCLUDE_DIR}")
        include_directories(${LIBUV_INCLUDE_DIR})
    else ()
        message(FATAL_ERROR "-- [${PROJECT_NAME}] libuv is needed for ${PROJECT_NAME} build")
    endif ()
endif()

# Optional Dependencies
# =============================================================================
if (HPC_AIO_LOGGER STREQUAL "CPP_LOGGER")
    find_package(cpp-logger REQUIRED
                 HINTS ${CPP_LOGGER_DIR} ${cpp-logger_DIR}
                       ${CPP_LOGGER_PATH} ${cpp-logger_PATH}
                       $ENV{CPP_LOGGER_DIR} $ENV{cpp-logger_DIR}
                       $ENV{CPP_LOGGER_PATH} $ENV{cpp-logger_PATH})
    if (${CPP_LOGGER_FOUND})
        message(STATUS "[${PROJECT_NAME}] found cpp-logger at ${CPP_LOGGER_INCLUDE_DIRS}")
        include_directories(${CPP_LOGGER_INCLUDE_DIRS})
        set(DEPENDENCY_LIB ${DEPENDENCY_LIB} ${CPP_LOGGER_LIBRARIES})
    else ()
        message(FATAL_ERROR "-- [${PROJECT_NAME}] cpp-logger is not found but selected in cmake options for ${PROJECT_NAME} build")
    endif ()
endif()
if(HPC_AIO_PROFILER STREQUAL "DLIO_PROFILER")
    find_package(dlio_profiler REQUIRED IMPORTED)
    if (${DLIO_PROFILER_FOUND})
        message(STATUS "[${PROJECT_NAME}] found dlio_profiler at ${DLIO_PROFILER_INCLUDE_DIRS}")
        include_directories(${DLIO_PROFILER_INCLUDE_DIRS})
        set(DEPENDENCY_LIB ${DEPENDENCY_LIB} ${DLIO_PROFILER_LIBRARIES})
    else ()
        message(FATAL_ERROR "-- [${PROJECT_NAME}] dlio_profiler is not found but selected in cmake options ${PROJECT_NAME} build")
    endif ()
endif()


function(hpc_aio_install_headers public_headers current_dir)
    message("-- [${PROJECT_NAME}] " "installing headers ${public_headers}")
    foreach (header ${public_headers})
        file(RELATIVE_PATH header_file_path "${PROJECT_SOURCE_DIR}/src" "${header}")
        message("-- [${PROJECT_NAME}] " "installing header ${header_file_path}")
        get_filename_component(header_directory_path "${header_file_path}" DIRECTORY)
        install(
                FILES ${header}
                DESTINATION "include/${header_directory_path}"
        )
        file(COPY ${header}
                DESTINATION "${CMAKE_INCLUDE_OUTPUT_DIRECTORY}/${header_directory_path}")
    endforeach ()
endfunction()

include_directories(${CMAKE_SOURCE_DIR}/include)  # public header
add_subdirectory(src/hpc_aio)
#cmake_policy(SET CMP0079 NEW) # In case that we need more control over the target building order

if(HPC_AIO_ENABLE_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()

#-----------------------------------------------------------------------------
# Configure the config.cmake file for the build directory
#-----------------------------------------------------------------------------
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/configure_files/${PROJECT_NAME}-config.cmake.build.in
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake/${PROJECT_NAME}/${PROJECT_NAME}-config.cmake @ONLY
)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/configure_files/${PROJECT_NAME}-config.cmake.install.in
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake/${PROJECT_NAME}/install/${PROJECT_NAME}-config.cmake @ONLY
)
install(
        FILES
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake/${PROJECT_NAME}/install/${PROJECT_NAME}-config.cmake
        DESTINATION
        ${HPC_AIO_LIBDIR}/cmake/${PROJECT_NAME}
)
#-----------------------------------------------------------------------------
# Configure the ${PROJECT_NAME}-config-version .cmake file for the install directory
#-----------------------------------------------------------------------------
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/configure_files/${PROJECT_NAME}-config-version.cmake.in
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake/${PROJECT_NAME}/${PROJECT_NAME}-config-version.cmake @ONLY
)

install(
        FILES
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake/${PROJECT_NAME}/${PROJECT_NAME}-config-version.cmake
        DESTINATION
        ${HPC_AIO_LIBDIR}/cmake/${PROJECT_NAME}
)

export(EXPORT ${HPC_AIO_EXPORTED_TARGETS}
        FILE "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake/${PROJECT_NAME}/${PROJECT_NAME}Targets.cmake")

# Write the configure file
configure_file("${CMAKE_SOURCE_DIR}/cmake/configure_files/hpc_aio_config.hpp.in"
               "${CMAKE_INCLUDE_OUTPUT_DIRECTORY}/hpc_aio/hpc_aio_config.hpp" @ONLY)


################################################################
# Install HPC_AIO
################################################################

include(CMakePackageConfigHelpers)

# Write the version file. This is independent of build/install tree.
write_basic_package_version_file(
  HPC_AIOConfigVersion.cmake
  VERSION "${HPC_AIO_PACKAGE_VERSION}"
  COMPATIBILITY SameMajorVersion)

install(FILES
        "${CMAKE_INCLUDE_OUTPUT_DIRECTORY}/hpc_aio/hpc_aio_config.hpp"
        DESTINATION
        ${HPC_AIO_INSTALL_INCLUDE_DIR}/hpc_aio)

install(EXPORT
        ${HPC_AIO_EXPORTED_TARGETS}
        DESTINATION
        ${HPC_AIO_LIBDIR}/cmake/${PROJECT_NAME}
        FILE
        ${HPC_AIO_EXPORTED_TARGETS}.cmake
        )

# Install license and readme
install(FILES
        "${PROJECT_SOURCE_DIR}/NOTICE"
        "${PROJECT_SOURCE_DIR}/LICENSE"
        "${PROJECT_SOURCE_DIR}/README.md"
        DESTINATION
        ${HPC_AIO_INSTALL_DOCDIR})


###############################################################################
#                      Print out configuration summary
###############################################################################
include(hpc_aio-utils)
if (CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
  set(HPC_AIO_GNU_LINUX TRUE)
endif ()

# NOTE: message() outputs to stderr by default. We now use a string to
# maintain this information and then have cmake echo it to stdout. The
# only side effects are that if you use the CMake GUI, you won't see
# this output anymore (they only report stderr) and that if you add
# something to the list, you must remember your newline!
set(_str "\n== HPC_AIO Configuration Summary ==\n\n")
string(APPEND _str "  PROJECT_SOURCE_DIR:   ${PROJECT_SOURCE_DIR}\n"
  "  PROJECT_BINARY_DIR:   ${PROJECT_BINARY_DIR}\n\n"
  "  CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}\n"
  "  CMAKE_BUILD_TYPE:     ${CMAKE_BUILD_TYPE}\n\n")
if (CMAKE_BUILD_TYPE MATCHES None)
  string(APPEND _str
    "  CXX FLAGS:            ${CMAKE_CXX_FLAGS}\n")
elseif (CMAKE_BUILD_TYPE MATCHES Release)
  string(APPEND _str
    "  CXX FLAGS:            ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}\n")
  string(APPEND _str
    "  C FLAGS:              ${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_RELEASE}\n")
elseif (CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
  string(APPEND _str
    "  CXX FLAGS:            ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}\n")
  string(APPEND _str
    "  C FLAGS:              ${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_RELWITHDEBINFO}\n")
elseif (CMAKE_BUILD_TYPE MATCHES Debug)
  string(APPEND _str
    "  CXX FLAGS:            ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}\n")
  string(APPEND _str
    "  C FLAGS:              ${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_DEBUG}\n")
endif ()
string(APPEND _str "\n")

string(APPEND _str
  "  HPC_AIO_PACKAGE_VERSION:        ${HPC_AIO_PACKAGE_VERSION}\n")
string(APPEND _str
  "  HPC_AIO_GIT_VERSION:            ${HPC_AIO_GIT_VERSION}\n")
string(APPEND _str
  "  HPC_AIO_COMMUNICATION:          ${HPC_AIO_COMMUNICATION}\n")
string(APPEND _str
  "  HPC_AIO_RUNTIME:                ${HPC_AIO_RUNTIME}\n")
string(APPEND _str
  "  HPC_AIO_PROFILER:               ${HPC_AIO_PROFILER}\n")
string(APPEND _str
  "  HPC_AIO_LOGGER:                 ${HPC_AIO_LOGGER}\n")
string(APPEND _str
  "  HPC_AIO_LOGGER_LEVEL:           ${HPC_AIO_LOGGER_LEVEL}\n")
string(APPEND _str
  "  GLIBC_VERSION:               ${GLIBC_VERSION}\n")

#Print the true/false variables
append_str_tf(_str
  HPC_AIO_GNU_LINUX
  HPC_AIO_COMMUNICATION_ENABLE_UCX
  HPC_AIO_RUNTIME_ENABLE_LIBUV
  HPC_AIO_LIBDIR_AS_LIB
  HPC_AIO_USE_CLANG_LIBCXX
  HPC_AIO_WARNINGS_AS_ERRORS
  HPC_AIO_ENABLE_TESTING
  HPC_AIO_ENABLE_POC_TESTING
)
string(APPEND _str
  "\n== End HPC_AIO Configuration Summary ==\n")

# Output to stdout
execute_process(COMMAND ${CMAKE_COMMAND} -E echo "${_str}")
set(_str)


###############################################################################
# Write a basic modulefile
###############################################################################

set(HPC_AIO_MODULEFILE_NAME "hpc_aio-${HPC_AIO_PACKAGE_VERSION}.lua")
#  CACHE STRING
#  "The name of the HPC_AIO modulefile to install. Must end in .lua.")

if (NOT (HPC_AIO_MODULEFILE_NAME MATCHES ".+\.lua"))
  message(WARNING
    "HPC_AIO_MODULEFILE_NAME must have extension \".lua\". Appending.")
  set(HPC_AIO_MODULEFILE_NAME "${HPC_AIO_MODULEFILE_NAME}.lua"
    CACHE STRING "" FORCE)
endif ()

configure_file(
  "${CMAKE_SOURCE_DIR}/cmake/configure_files/hpc_aio_module.lua.in"
  "${CMAKE_BINARY_DIR}/hpc_aio_module.lua.install"
  @ONLY)

install(FILES "${CMAKE_BINARY_DIR}/hpc_aio_module.lua.install"
  RENAME "${HPC_AIO_MODULEFILE_NAME}"
  DESTINATION
  "${HPC_AIO_INSTALL_SYSCONFDIR}")