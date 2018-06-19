
# Photon-v2 editor's JNI interface.

find_package(JNI)
if(NOT JNI_FOUND)
	message(WARNING 
		"JDK not found, JNI related functionalities will not be available"
	)
	return()
endif()

file(GLOB_RECURSE Editor_JNI_HEADER_FILES 
	"./Include/*.h" 
	"./Source/*.h" 
	"./Source/*.ipp" 
	"./Source/*.tpp"
)
file(GLOB_RECURSE Editor_JNI_SOURCE_FILES "./Source/*.cpp")

set_source_files_properties(${Editor_JNI_HEADER_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(Editor_JNI SHARED ${Editor_JNI_HEADER_FILES} ${Editor_JNI_SOURCE_FILES})

target_include_directories(Editor_JNI
	PRIVATE "./Source/"
	PRIVATE ${ENGINE_INCLUDE_DIR}
	PRIVATE ${JAVA_INCLUDE_PATH} 
	PRIVATE ${JAVA_INCLUDE_PATH2}
)

add_dependencies(Editor_JNI Engine)
target_link_libraries(Editor_JNI Engine)
