# Copyright (c) 2007-2016 Hartmut Kaiser
# Copyright (c) 2011      Bryce Adelstein-Lelbach
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

set(example_programs
    1d_wave_equation
    allow_unknown_options
    command_line_handling
    component_ctors
    component_in_executable
    component_inheritance
    component_with_executor
    composable_guard
    customize_async
    data_actions
    enumerate_threads
    error_handling
    event_synchronization
    factorial
    fibonacci
    fibonacci_await
    fibonacci_dataflow
    fibonacci_one
    fibonacci_futures
    fibonacci_futures_distributed
    fibonacci_local
    file_serialization
    fractals
    fractals_struct
    hello_world
    init_globally
    interest_calculator
    interval_timer
    latch_local
    latch_remote
    local_channel
    partitioned_vector_spmd_foreach
    pingpong
    pipeline1
    print_to_console
    potpourri
    receive_buffer
    safe_object
    shared_mutex
    sierpinski
    simple_future_continuation
    simplest_hello_world_1
    simplest_hello_world_2
    timed_futures
    timed_wake
    use_main_thread
    vector_counting_dotproduct
    vector_zip_dotproduct
    wait_composition
    zerocopy_rdma
   )

# TODO: These examples currently fail. Disabled until they are fixed.
set(disabled_tests
    error_handling
    fibonacci_await
    non_atomic_rma
    shared_mutex
    zerocopy_rdma
   )

set(1d_wave_equation_FLAGS COMPONENT_DEPENDENCIES iostreams)
set(allow_unknown_options_FLAGS COMPONENT_DEPENDENCIES iostreams)
set(command_line_handling_FLAGS COMPONENT_DEPENDENCIES iostreams)
set(component_in_executable_FLAGS COMPONENT_DEPENDENCIES iostreams)
set(component_ctors_FLAGS COMPONENT_DEPENDENCIES iostreams)
set(component_inheritance_FLAGS COMPONENT_DEPENDENCIES iostreams)
set(component_with_executor_FLAGS COMPONENT_DEPENDENCIES iostreams)
set(customize_async_FLAGS COMPONENT_DEPENDENCIES iostreams)
set(enumerate_threads_FLAGS COMPONENT_DEPENDENCIES iostreams)
set(event_synchronization_FLAGS COMPONENT_DEPENDENCIES iostreams)
set(error_handling_FLAGS COMPONENT_DEPENDENCIES iostreams)
set(fractals_FLAGS COMPONENT_DEPENDENCIES iostreams)
if(HPX_HAVE_COMPRESSION_SNAPPY AND SNAPPY_FOUND)
  set(hello_world_FLAGS COMPONENT_DEPENDENCIES iostreams DEPENDENCIES compress_snappy)
else()
  set(hello_world_FLAGS COMPONENT_DEPENDENCIES iostreams)
endif()
set(interval_timer_FLAGS COMPONENT_DEPENDENCIES iostreams)
set(local_channel_FLAGS COMPONENT_DEPENDENCIES iostreams)
set(sierpinski_FLAGS COMPONENT_DEPENDENCIES iostreams)
set(partitioned_vector_spmd_foreach_FLAGS
    COMPONENT_DEPENDENCIES iostreams partitioned_vector)
set(pingpong_FLAGS COMPONENT_DEPENDENCIES iostreams)
set(potpourri_FLAGS COMPONENT_DEPENDENCIES iostreams)
set(shared_mutex_FLAGS COMPONENT_DEPENDENCIES iostreams)
set(simplest_hello_world_1_FLAGS COMPONENT_DEPENDENCIES iostreams)
set(simplest_hello_world_2_FLAGS COMPONENT_DEPENDENCIES iostreams)
set(simple_future_continuation_FLAGS COMPONENT_DEPENDENCIES iostreams)
set(timed_futures_FLAGS COMPONENT_DEPENDENCIES iostreams)
set(vector_counting_dotproduct_FLAGS COMPONENT_DEPENDENCIES iostreams)
set(vector_zip_dotproduct_FLAGS COMPONENT_DEPENDENCIES iostreams)

set(1d_wave_equation_PARAMETERS THREADS_PER_LOCALITY 4 "-Ihpx.stacks.use_guard_pages=0")
set(allow_unknown_options_PARAMETERS THREADS_PER_LOCALITY 4)
set(command_line_handling_PARAMETERS THREADS_PER_LOCALITY 4)
set(component_in_executable_PARAMETERS THREADS_PER_LOCALITY 4)
set(component_ctors_PARAMETERS THREADS_PER_LOCALITY 4)
set(component_inheritance_PARAMETERS THREADS_PER_LOCALITY 4)
set(component_with_executor_PARAMETERS THREADS_PER_LOCALITY 4)
set(customize_async_PARAMETERS THREADS_PER_LOCALITY 4)
set(enumerate_threads_PARAMETERS THREADS_PER_LOCALITY 4)
set(event_synchronization_PARAMETERS THREADS_PER_LOCALITY 4)
set(error_handling_PARAMETERS THREADS_PER_LOCALITY 4)
set(fractals_PARAMETERS THREADS_PER_LOCALITY 4)
set(hello_world_PARAMETERS THREADS_PER_LOCALITY 4)
set(interval_timer_PARAMETERS THREADS_PER_LOCALITY 4)
set(local_channel_PARAMETERS THREADS_PER_LOCALITY 4)
set(sierpinski_PARAMETERS THREADS_PER_LOCALITY 4)
set(partitioned_vector_spmd_foreach_PARAMETERS THREADS_PER_LOCALITY 4)
set(pingpong_PARAMETERS THREADS_PER_LOCALITY 4)
set(simplest_hello_world_PARAMETERS THREADS_PER_LOCALITY 4)
set(simple_future_continuation_PARAMETERS THREADS_PER_LOCALITY 4)
set(timed_futures_PARAMETERS THREADS_PER_LOCALITY 4)
set(vector_counting_dotproduct_PARAMETERS THREADS_PER_LOCALITY 4)
set(vector_zip_dotproduct_PARAMETERS THREADS_PER_LOCALITY 4)

if(HPX_WITH_LOCAL_SCHEDULER OR HPX_WITH_ALL_SCHEDULERS)
  set(example_programs ${example_programs}
      fractals_executor
     )
  set(fractals_executor_FLAGS COMPONENT_DEPENDENCIES iostreams)
set(fractals_executor_PARAMETERS THREADS_PER_LOCALITY 4)
endif()

if(HPX_WITH_TUPLE_RVALUE_SWAP)
  set(example_programs ${example_programs}
      sort_by_key_demo
     )
  set(sort_by_key_demo_FLAGS COMPONENT_DEPENDENCIES iostreams)
endif()

foreach(example_program ${example_programs})
  set(sources
      ${example_program}.cpp)

  source_group("Source Files" FILES ${sources})

  # add example executable
  add_hpx_executable(${example_program}
                     SOURCES ${sources}
                     ${${example_program}_FLAGS}
                     FOLDER "Examples/Quickstart")

  # add a custom target for this example
  add_hpx_pseudo_target(examples.quickstart.${example_program})

  # make pseudo-targets depend on master pseudo-target
  add_hpx_pseudo_dependencies(examples.quickstart
                              examples.quickstart.${example_program})

  # add dependencies to pseudo-target
  add_hpx_pseudo_dependencies(examples.quickstart.${example_program}
                              ${example_program})

  if(HPX_WITH_TESTS AND HPX_WITH_TESTS_EXAMPLES AND
      NOT ("${example_program}" IN_LIST disabled_tests))
    add_hpx_example_test("quickstart" ${example_program} ${${example_program}_PARAMETERS})
    add_hpx_pseudo_target(tests.examples.quickstart.${example_program})
    add_hpx_pseudo_dependencies(tests.examples.quickstart
                                tests.examples.quickstart.${example_program})
    add_hpx_pseudo_dependencies(tests.examples.quickstart.${example_program}
                                ${example_program})
  endif()
endforeach()
