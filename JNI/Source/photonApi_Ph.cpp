#include "photonApi_Ph.h"
#include "ph_test.h"
#include "JIntRef.h"
#include "JLongRef.h"

#include <iostream>

jboolean JNICALL Java_photonApi_Ph_init(JNIEnv* env, jclass thiz)
{
	std::cout << "initializing Photon..." << std::endl;

	// TODO

	std::cout << "initializing Photon JNI..." << std::endl;
	ph::JIntRef::initNativeJavaClass(env);
	ph::JLongRef::initNativeJavaClass(env);

	// HACK
	return JNI_TRUE;
}

void JNICALL Java_photonApi_Ph_printTestMessage(JNIEnv* env, jclass thiz)
{
	std::cout << "hi from C++" << std::endl;
}

void JNICALL Java_photonApi_Ph_genTestHdrFrame(JNIEnv* env, jclass thiz, jobject out_pixelData, jobject out_widthPx, jobject out_heightPx)
{
	const PHfloat32* pixelData;
	PHuint32 width;
	PHuint32 height;
	genTestHdrFrame(&pixelData, &width, &height);

	//std::cout << "width: " << width << ", height: " << height << std::endl;
	ph::JIntRef jWidthPx(out_widthPx, env);
	ph::JIntRef jHeightPx(out_heightPx, env);
	jWidthPx.setValue(static_cast<PHint32>(width));
	jHeightPx.setValue(static_cast<PHint32>(height));

	/*jclass localRef = env->GetObjectClass(out_widthPx);
	jfieldID fieldId = env->GetFieldID(localRef, "m_value", "I");
	env->SetIntField(out_widthPx, fieldId, static_cast<jint>(width));*/

	jclass class_out_pixelData = env->GetObjectClass(out_pixelData);
	jfieldID arrayField = env->GetFieldID(class_out_pixelData, "m_value", "[F");
	
	const jsize arrayLength = static_cast<jsize>(width * height);
	jfloatArray arrayObject = env->NewFloatArray(arrayLength);
	env->SetFloatArrayRegion(arrayObject, 0, arrayLength, static_cast<const jfloat*>(pixelData));
	env->SetObjectField(out_pixelData, arrayField, arrayObject);
}