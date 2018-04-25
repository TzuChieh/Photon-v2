
# Photon-v2 JNI interface engine.

file(GLOB_RECURSE SOURCE_FILES "./Source/*.cpp")
add_library(JNI SHARED ${SOURCE_FILES})

find_package(JNI)

if(NOT JNI_FOUND)
	message(WARNING 
		"JDK not found, JNI related functionalities will not be available"
	)
	return()
endif()

target_include_directories(JNI
	PRIVATE "./Source/"
	PRIVATE ${ENGINE_INCLUDE_DIR}
	PRIVATE ${JAVA_INCLUDE_PATH} 
	PRIVATE ${JAVA_INCLUDE_PATH2}
)

add_dependencies(JNI Engine)

target_link_libraries(JNI Engine)
