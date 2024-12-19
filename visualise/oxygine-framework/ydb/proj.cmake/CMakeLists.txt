cmake_minimum_required (VERSION 2.6)
project (ActorVisualise)

add_subdirectory(../../ oxygine-framework)
add_definitions(${OXYGINE_DEFINITIONS})
include_directories(${OXYGINE_INCLUDE_DIRS})
link_directories(${OXYGINE_LIBRARY_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OXYGINE_CXX_FLAGS}")

add_executable(ActorVisualise ../src/FramedProgressBar.cpp ../src/Arrow.cpp ../src/ActorModel.cpp ../src/helper.cpp ../src/main.cpp ../src/ActorVisualise.cpp ../src/LogParser.cpp ../src/Unit.cpp  ../src/ActorVisualise.h ../src/ActorModel.h ../src/helper.h ../src/LogParser.h ../src/Unit.h ../src/Arrow.h ../src/FramedProgressBar.h ../src/StageInfo.h)
target_link_libraries(ActorVisualise ${OXYGINE_CORE_LIBS})



if (WIN32) #disable console mode for VC++
	set_target_properties(ActorVisualise PROPERTIES WIN32_EXECUTABLE TRUE)
endif(WIN32)



if (EMSCRIPTEN)
	SET(CMAKE_EXECUTABLE_SUFFIX ".html")	
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s NO_EXIT_RUNTIME=1 -s WASM=0 -s WARN_ON_UNDEFINED_SYMBOLS=1 --memory-init-file 0 -s TOTAL_MEMORY=50331648 -s FORCE_FILESYSTEM=1")
	em_link_pre_js(Demo  ${OXYGINE_JS_LIBRARIES}  ${CMAKE_CURRENT_SOURCE_DIR}/data.js)
endif(EMSCRIPTEN)