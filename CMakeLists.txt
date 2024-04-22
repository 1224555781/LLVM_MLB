set(LLVM_LINK_COMPONENTS
  AllTargetsDescs
  AllTargetsDisassemblers
  AllTargetsInfos
  BinaryFormat
  ExecutionEngine
  JITLink
  MC
  Object
  OrcJIT
  OrcShared
  OrcTargetProcess
  RuntimeDyld
  Support
  TargetParser
  )

add_llvm_tool(llvm-MLB
  Lexer.cpp
  AST.cpp
  Parser.cpp
  LaunchWindows.cpp
  )

if(${CMAKE_SYSTEM_NAME} MATCHES "Haiku")
  target_link_libraries(llvm-MLB PRIVATE network)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
  target_link_libraries(llvm-MLB PRIVATE socket nsl)
endif()

export_executable_symbols(llvm-MLB)
