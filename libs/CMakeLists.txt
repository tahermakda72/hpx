# Copyright (c) 2019 The STE||AR-Group
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# This file is auto generated. Please do not edit manually

include(HPX_CreateSymbolicLink)

# We create a special directory to collect all our modular headers, to make
# it easier to include those files. The directory is created from scratch if
# changes occured to avoid dangling links
execute_process(COMMAND "${CMAKE_COMMAND}" -E remove_directory ${CMAKE_BINARY_DIR}/include/hpx)
execute_process(COMMAND "${CMAKE_COMMAND}" -E make_directory ${CMAKE_BINARY_DIR}/include/hpx)

