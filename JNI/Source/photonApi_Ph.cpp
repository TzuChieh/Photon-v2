#include "photonApi_Ph.h"
#include "ph_test.h"
#include "JIntRef.h"
#include "JLongRef.h"
#include "ph_core.h"

#include <iostream>

jboolean JNICALL Java_photonApi_Ph_phStart(JNIEnv* env, jclass thiz)
{
	std::cout << "initializing Photon..." << std::endl;

	return phStart() == PH_TRUE ? JNI_TRUE : JNI_FALSE;
}

void JNICALL Java_photonApi_Ph_phExit(JNIEnv* env, jclass thiz)
{
	std::cout << "exiting Photon..." << std::endl;

	phExit();
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

	const jsize arrayLength = static_cast<jsize>(width * height * 3);

	jclass class_out_pixelData = env->GetObjectClass(out_FloatArrayRef_pixelData);
	jfieldID arrayField = env->GetFieldID(class_out_pixelData, "m_value", "[F");
	jfloatArray arrayObject = env->NewFloatArray(arrayLength);
	env->SetFloatArrayRegion(arrayObject, 0, arrayLength, static_cast<const jfloat*>(pixelData));
	env->SetObjectField(out_FloatArrayRef_pixelData, arrayField, arrayObject);
}

/*
* Class:     photonApi_Ph
* Method:    phCreateRenderer
* Signature: (LphotonApi/LongRef;I)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phCreateRenderer
(JNIEnv* env, jclass thiz, jobject out_LongRef_rendererId, jint rendererType)
{
	ph::JLongRef jRendererId(out_LongRef_rendererId, env);
	PHuint64 rendererId;

	switch(rendererType)
	{
	case photonApi_Ph_PH_BRUTE_FORCE_RENDERER_TYPE:
		phCreateRenderer(&rendererId, PH_BRUTE_FORCE_RENDERER_TYPE);
		break;

	case photonApi_Ph_PH_IMPORTANCE_RENDERER_TYPE:
		phCreateRenderer(&rendererId, PH_IMPORTANCE_RENDERER_TYPE);
		break;

	default:
		phCreateRenderer(&rendererId, static_cast<PHint32>(rendererType));
	}

	jRendererId.setValue(rendererId);
}

/*
* Class:     photonApi_Ph
* Method:    phDeleteRenderer
* Signature: (J)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phDeleteRenderer
(JNIEnv* env, jclass thiz, jlong rendererId)
{
	phDeleteRenderer(static_cast<PHuint64>(rendererId));
}

/*
* Class:     photonApi_Ph
* Method:    phCreateFrame
* Signature: (LphotonApi/LongRef;III)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phCreateFrame
(JNIEnv* env, jclass thiz, jobject out_LongRef_frameId, jint frameWidthPx, jint frameHeightPx, jint frameType)
{
	PHuint64 frameId;
	switch(frameType)
	{
	case photonApi_Ph_PH_HDR_FRAME_TYPE:
		phCreateFrame(&frameId, frameWidthPx, frameHeightPx, PH_HDR_FRAME_TYPE);
		break;

	default:
		std::cerr << "unknown frame type in Java_photonApi_Ph_phCreateFrame()" << std::endl;
	}

	ph::JLongRef jFrameId(out_LongRef_frameId, env);
	jFrameId.setValue(frameId);
}

/*
* Class:     photonApi_Ph
* Method:    phDeleteFrame
* Signature: (J)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phDeleteFrame
(JNIEnv* env, jclass thiz, jlong frameId)
{
	phDeleteFrame(static_cast<PHuint64>(frameId));
}