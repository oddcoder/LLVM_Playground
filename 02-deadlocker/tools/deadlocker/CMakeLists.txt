set(LLVM_USED_LIBS callgraphs)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(deadlocker
  main.cpp
  )

llvm_map_components_to_libnames(REQ_LLVM_LIBRARIES
        asmparser core linker bitreader irreader ipo
        analysis support Passes)

target_link_libraries(deadlocker DoubleLocking  callgraphs ${REQ_LLVM_LIBRARIES})

# Platform dependencies.
if( WIN32 )
  target_link_libraries(deadlocker
    shlwapi
  )
else()
  target_link_libraries(deadlocker
    pthread
    dl
    curses
  )
endif()

set_target_properties(deadlocker
                      PROPERTIES
                      LINKER_LANGUAGE CXX
                      PREFIX "")

install(TARGETS deadlocker
  RUNTIME DESTINATION bin)

