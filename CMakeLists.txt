add_library(
  src
  src/lr-node.cc
  src/lr-node-container.cc
  src/lr-routing-protocol.cc
  src/simulation-helper.cc
  )

build_exec(
  EXECNAME lra-simulator
  SOURCE_FILES lra-simulator.cc
  LIBRARIES_TO_LINK src
                    ${libcore} ${ns3-libs} ${ns3-contrib-libs}

  EXECUTABLE_DIRECTORY_PATH ${CMAKE_OUTPUT_DIRECTORY}/scratch/
)
