CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)

file(GLOB SOURCES_TEST ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)
foreach(local_file ${SOURCES_TEST})
  get_filename_component(local_filename ${local_file} NAME_WE)

  add_executable(${local_filename} ${local_file})
  target_link_libraries(${local_filename} readlinepp
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY})

  add_test(UnitTests ${local_filename})
endforeach()
