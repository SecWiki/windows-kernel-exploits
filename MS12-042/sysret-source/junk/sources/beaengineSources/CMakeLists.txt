set (BEA_SOURCES BeaEngine.c)

if (optBUILD_DLL)
  add_library (${BEA_TARGET} SHARED BeaEngine.c)
  set_target_properties (${BEA_TARGET} PROPERTIES COMPILE_FLAGS "-DBUILD_BEA_ENGINE_DLL") 
else ()
  add_library (${BEA_TARGET} STATIC BeaEngine.c)
  set_target_properties (${BEA_TARGET} PROPERTIES COMPILE_FLAGS "-DBEA_ENGINE_STATIC") 
endif ()
