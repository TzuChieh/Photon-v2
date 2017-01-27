#include "photonApi_Ph.h"
#include "JIntRef.h"
#include "JLongRef.h"
#include "JFloatRef.h"

#include "ph_test.h"
#include "ph_core.h"
#include "ph_query.h"

#include <iostream>

/*
* Class:     photonApi_Ph
* Method:    phStart
* Signature: ()Z
*/
JNIEXPORT jboolean JNICALL Java_photonApi_Ph_phStart
(JNIEnv* env, jclass thiz)
{
	std::cout << "initializing Photon..." << std::endl;

	return phStart() == PH_TRUE ? JNI_TRUE : JNI_FALSE;
}

/*
* Class:     photonApi_Ph
* Method:    phExit
* Signature: ()V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phExit
(JNIEnv* env, jclass thiz)
{
	std::cout << "exiting Photon..." << std::endl;

	phExit();
}

/*
* Class:     photonApi_Ph
* Method:    phCreateRenderer
* Signature: (LphotonApi/LongRef;II)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phCreateRenderer
(JNIEnv* env, jclass thiz, jobject out_LongRef_rendererId, jint numThreads)
{
	ph::JLongRef jRendererId(out_LongRef_rendererId, env);
	PHuint64 rendererId;

	phCreateRenderer(&rendererId, static_cast<PHuint32>(numThreads));

	jRendererId.setValue(static_cast<PHint64>(rendererId));
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
(JNIEnv* env, jclass thiz, jobject out_LongRef_frameId, jint frameType)
{
	PHuint64 frameId;
	switch(frameType)
	{
	case photonApi_Ph_PH_HDR_FRAME_TYPE:
		phCreateFrame(&frameId, PH_HDR_FRAME_TYPE);
		break;

	default:
		std::cerr << "unknown frame type in Java_photonApi_Ph_phCreateFrame()" << std::endl;
	}

	ph::JLongRef jFrameId(out_LongRef_frameId, env);
	jFrameId.setValue(static_cast<PHint64>(frameId));
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

/*
* Class:     photonApi_Ph
* Method:    phCreateDescription
* Signature: (LphotonApi/LongRef;)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phCreateDescription
(JNIEnv* env, jclass thiz, jobject out_LongRef_descriptionId)
{
	ph::JLongRef jDescriptionId(out_LongRef_descriptionId, env);

	PHuint64 descriptionId;
	phCreateDescription(&descriptionId);

	jDescriptionId.setValue(static_cast<PHint64>(descriptionId));
}

/*
* Class:     photonApi_Ph
* Method:    phLoadDescription
* Signature: (JLjava/lang/String;)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phLoadDescription
(JNIEnv* env, jclass thiz, jlong descriptionId, jstring filename)
{
	const char* javaString = env->GetStringUTFChars(filename, JNI_FALSE);
	phLoadDescription(static_cast<PHuint64>(descriptionId), javaString);
	env->ReleaseStringUTFChars(filename, javaString);
}

/*
* Class:     photonApi_Ph
* Method:    phUpdateDescription
* Signature: (J)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phUpdateDescription
(JNIEnv* env, jclass thiz, jlong descriptionId)
{
	phUpdateDescription(static_cast<PHuint64>(descriptionId));
}

/*
* Class:     photonApi_Ph
* Method:    phDevelopFilm
* Signature: (JJ)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phDevelopFilm
(JNIEnv* env, jclass thiz, jlong descriptionId, jlong frameId)
{
	phDevelopFilm(static_cast<PHuint64>(descriptionId), static_cast<PHuint64>(frameId));
}

/*
* Class:     photonApi_Ph
* Method:    phDeleteDescription
* Signature: (J)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phDeleteDescription
(JNIEnv* env, jclass thiz, jlong descriptionId)
{
	phDeleteDescription(static_cast<PHuint64>(descriptionId));
}

/*
* Class:     photonApi_Ph
* Method:    phRender
* Signature: (JJ)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phRender
(JNIEnv* env, jclass thiz, jlong rendererId, jlong descriptionId)
{
	phRender(static_cast<PHuint64>(rendererId), static_cast<PHuint64>(descriptionId));
}

/*
* Class:     photonApi_Ph
* Method:    phGetFrameData
* Signature: (JLphotonApi/FloatArrayRef;LphotonApi/IntRef;LphotonApi/IntRef;LphotonApi/IntRef;)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phGetFrameData
(JNIEnv* env, jclass thiz, jlong frameId, jobject out_FloatArrayRef_pixelData, 
                                          jobject out_IntRef_widthPx, 
                                          jobject out_IntRef_heightPx, 
                                          jobject out_IntRef_nPixelComponents)
{
	const PHfloat32* pixelData;
	PHuint32 widthPx;
	PHuint32 heightPx;
	PHuint32 nPixelComponents;
	phGetFrameData(static_cast<PHuint64>(frameId), &pixelData, &widthPx, &heightPx, &nPixelComponents);

	const jsize arrayLength = static_cast<jsize>(widthPx * heightPx * nPixelComponents);

	jclass class_out_pixelData = env->GetObjectClass(out_FloatArrayRef_pixelData);
	jfieldID valueField = env->GetFieldID(class_out_pixelData, "m_value", "[F");
	jfloatArray jArrayObject = env->NewFloatArray(arrayLength);
	env->SetFloatArrayRegion(jArrayObject, 0, arrayLength, static_cast<const jfloat*>(pixelData));
	env->SetObjectField(out_FloatArrayRef_pixelData, valueField, jArrayObject);

	ph::JIntRef jWidthPx(out_IntRef_widthPx, env);
	ph::JIntRef jHeightPx(out_IntRef_heightPx, env);
	ph::JIntRef jnPixelComponents(out_IntRef_nPixelComponents, env);
	jWidthPx.setValue(static_cast<PHint32>(widthPx));
	jHeightPx.setValue(static_cast<PHint32>(heightPx));
	jnPixelComponents.setValue(static_cast<PHint32>(nPixelComponents));
}

/*
* Class:     photonApi_Ph
* Method:    phQueryRendererPercentageProgress
* Signature: (JLphotonApi/FloatRef;)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phQueryRendererPercentageProgress
(JNIEnv* env, jclass thiz, jlong rendererId, jobject out_FloatRef_progress)
{
	PHfloat32 progress;
	phQueryRendererPercentageProgress(static_cast<PHuint64>(rendererId), &progress);
	ph::JFloatRef jProgress(out_FloatRef_progress, env);
	jProgress.setValue(progress);
}

/*
* Class:     photonApi_Ph
* Method:    phQueryRendererSampleFrequency
* Signature: (JLphotonApi/FloatRef;)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phQueryRendererSampleFrequency
(JNIEnv* env, jclass thiz, jlong rendererId, jobject out_FloatRef_frequency)
{
	PHfloat32 frequency;
	phQueryRendererSampleFrequency(static_cast<PHuint64>(rendererId), &frequency);
	ph::JFloatRef jFrequency(out_FloatRef_frequency, env);
	jFrequency.setValue(frequency);
}