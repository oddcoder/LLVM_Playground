add_library(pathprofiler-inst
  InnermostLoops.cpp
  PathEncodingPass.cpp
  PathProfilingPass.cpp
  ProfileDecodingPass.cpp
)

add_library(pathprofiler-lib MODULE
  InnermostLoops.cpp
  PathEncodingPass.cpp
  PathProfilingPass.cpp
  ProfileDecodingPass.cpp
)
