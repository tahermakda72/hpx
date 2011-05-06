# Copyright (c) 2007-2011 Hartmut Kaiser
# Copyright (c) 2011      Bryce Lelbach
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying 
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

set(HPX_ADDEXECUTABLE_LOADED TRUE)

include(HPX_Include)

hpx_include(Message
            ParseArguments
            Install)

macro(add_hpx_executable name)
  # retrieve arguments
  hpx_parse_arguments(${name}
    "MODULE;SOURCES;HEADERS;DEPENDENCIES" "ESSENTIAL;NOLIBS" ${ARGN})

  hpx_print_list("DEBUG" "add_executable.${name}" "Sources for ${name}" ${name}_SOURCES)
  hpx_print_list("DEBUG" "add_executable.${name}" "Headers for ${name}" ${name}_HEADERS)
  hpx_print_list("DEBUG" "add_executable.${name}" "Dependencies for ${name}" ${name}_DEPENDENCIES)

  # add the executable build target
  if(${name}_ESSENTIAL)
    add_executable(${name}_exe 
      ${${name}_SOURCES} ${${name}_HEADERS})
  else()
    add_executable(${name}_exe EXCLUDE_FROM_ALL
      ${${name}_SOURCES} ${${name}_HEADERS})
  endif()

  set_target_properties(${name}_exe PROPERTIES OUTPUT_NAME ${name})

  set_property(TARGET ${name}_exe APPEND
               PROPERTY COMPILE_DEFINITIONS
               "HPX_APPLICATION_NAME=${name}"
               "HPX_APPLICATION_STRING=\"${name}\""
               "HPX_APPLICATION_EXPORTS")

  set(libs "")
  if(NOT MSVC)
    set(libs ${BOOST_FOUND_LIBRARIES})
  endif()

  # linker instructions
  if(NOT ${name}_NOLIBS)
    target_link_libraries(${name}_exe
      ${${name}_DEPENDENCIES} 
      ${hpx_LIBRARIES}
      ${libs}
      ${pxaccel_LIBRARIES})
  else()
    target_link_libraries(${name}_exe
      ${${name}_DEPENDENCIES})
  endif()

  if(NOT ${name}_MODULE)
    set(${name}_MODULE "Unspecified")
    hpx_debug("add_executable.${name}" "Module was not specified for executable.")
  endif()

  if(${name}_ESSENTIAL)
    hpx_executable_install(${name}_exe MODULE ${${name}_MODULE} ESSENTIAL)
  else()
    hpx_executable_install(${name}_exe MODULE ${${name}_MODULE})
  endif() 
endmacro()

