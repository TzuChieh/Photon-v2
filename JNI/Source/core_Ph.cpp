#include "core_Ph.h"

#include "ph_test.h"

#include <iostream>

void JNICALL Java_core_Ph_printTestMessage(JNIEnv* env, jclass thiz)
{
	std::cout << "hi from C++" << std::endl;
}

void JNICALL Java_core_Ph_genTestHdrFrame(JNIEnv* env, jclass thiz, jobject out_pixelData, jobject out_widthPx, jobject out_heightPx)
{
	const PHfloat32* pixelData;
	PHuint32 width;
	PHuint32 height;
	genTestHdrFrame(&pixelData, &width, &height);

	std::cout << "width: " << width << ", height: " << height << std::endl;

	jclass class_out_pixelData = env->GetObjectClass(out_pixelData);
	jfieldID arrayField = env->GetFieldID(class_out_pixelData, "m_value", "[F");
	
	const jsize arrayLength = static_cast<jsize>(width * height);
	jfloatArray arrayObject = env->NewFloatArray(arrayLength);
	env->SetFloatArrayRegion(arrayObject, 0, arrayLength, static_cast<const jfloat*>(pixelData));
	env->SetObjectField(out_pixelData, arrayField, arrayObject);


}