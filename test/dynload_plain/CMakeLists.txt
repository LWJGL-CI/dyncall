add_executable(dynload_plain dynload_plain.c)
target_link_libraries(dynload_plain dynload_s ${CMAKE_DL_LIBS})

