#include "photonApi_Ph.h"
#include "ph_test.h"
#include "JIntRef.h"
#include "JLongRef.h"
#include "ph_frame.h"

#include <iostream>

jboolean JNICALL Java_photonApi_Ph_init(JNIEnv* env, jclass thiz)
{
	std::cout << "initializing Photon..." << std::endl;

	// TODO

	std::cout << "initializing Photon JNI..." << std::endl;
	//ph::JIntRef::initNativeJavaClass(env);
	//ph::JLongRef::initNativeJavaClass(env);

	// HACK
	return JNI_TRUE;
}

void JNICALL Java_photonApi_Ph_printTestMessage(JNIEnv* env, jclass thiz)
{
	std::cout << "hi from C++" << std::endl;
}

void JNICALL Java_photonApi_Ph_genTestHdrFrame(JNIEnv* env, jclass thiz, jobject out_FloatArrayRef_pixelData, jobject out_IntRef_widthPx, jobject out_IntRef_heightPx)
{
	const PHfloat32* pixelData;
	PHuint32 width;
	PHuint32 height;
	genTestHdrFrame(&pixelData, &width, &height);

	//std::cout << "width: " << width << ", height: " << height << std::endl;
	ph::JIntRef jWidthPx(out_IntRef_widthPx, env);
	ph::JIntRef jHeightPx(out_IntRef_heightPx, env);
	jWidthPx.setValue(static_cast<PHint32>(width));
	jHeightPx.setValue(static_cast<PHint32>(height));

	jclass class_out_pixelData = env->GetObjectClass(out_FloatArrayRef_pixelData);
	jfieldID arrayField = env->GetFieldID(class_out_pixelData, "m_value", "[F");
	
	const jsize arrayLength = static_cast<jsize>(width * height);
	jfloatArray arrayObject = env->NewFloatArray(arrayLength);
	env->SetFloatArrayRegion(arrayObject, 0, arrayLength, static_cast<const jfloat*>(pixelData));
	env->SetObjectField(out_FloatArrayRef_pixelData, arrayField, arrayObject);
}

void JNICALL Java_photonApi_Ph_createHdrFrame(JNIEnv* env, jclass thiz, jobject out_IntRef_frameId, jint widthPx, jint heightPx)
{
	PHuint64 frameId;
	phCreateHdrFrame(&frameId, static_cast<PHuint32>(widthPx), static_cast<PHuint32>(heightPx));
	ph::JLongRef jFrameId(out_IntRef_frameId, env);
	jFrameId.setValue(frameId);
}

void JNICALL Java_photonApi_Ph_deleteHdrFrame(JNIEnv* env, jclass thiz, jlong frameId)
{
	phDeleteHdrFrame(static_cast<PHuint64>(frameId));
}