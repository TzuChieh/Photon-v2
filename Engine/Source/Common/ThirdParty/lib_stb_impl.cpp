#include "Common/compiler.h"

///////////////////////////////////////////////////////////////////////////////
// using <stb_image.h>

// to create implementation
//
#define STB_IMAGE_IMPLEMENTATION

// to getReference slightly more user-friendly failure message
//
#define STBI_FAILURE_USERMSG

#include <stb_image.h>

///////////////////////////////////////////////////////////////////////////////
// using <stb_image_write.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION

#ifdef PH_COMPILER_IS_MSVC
	#define STBI_MSC_SECURE_CRT
#endif

#include <stb_image_write.h>
