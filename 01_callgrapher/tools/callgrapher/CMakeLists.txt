set(LLVM_USED_LIBS callgraphs)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(callgrapher
  main.cpp
  )

llvm_map_components_to_libnames(REQ_LLVM_LIBRARIES
        asmparser core linker bitreader irreader ipo
        analysis support Passes)

target_link_libraries(callgrapher callgraphs ${REQ_LLVM_LIBRARIES})

# Platform dependencies.
if( WIN32 )
  target_link_libraries(callgrapher
    shlwapi
  )
else()
  target_link_libraries(callgrapher
    pthread
    dl
    curses
  )
endif()

set_target_properties(callgrapher
                      PROPERTIES
                      LINKER_LANGUAGE CXX
                      PREFIX "")

install(TARGETS callgrapher
  RUNTIME DESTINATION bin)

