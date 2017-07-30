#include "photonApi_Ph.h"
#include "JIntRef.h"
#include "JLongRef.h"
#include "JFloatRef.h"
#include "JniUtil.h"

#include <ph_core.h>

#include <iostream>

/*
* Class:     photonApi_Ph
* Method:    phInit
* Signature: ()Z
*/
JNIEXPORT jboolean JNICALL Java_photonApi_Ph_phInit
(JNIEnv* env, jclass thiz)
{
	return phInit() == PH_TRUE ? JNI_TRUE : JNI_FALSE;
}

/*
* Class:     photonApi_Ph
* Method:    phExit
* Signature: ()Z
*/
JNIEXPORT jboolean JNICALL Java_photonApi_Ph_phExit
(JNIEnv* env, jclass thiz)
{
	return phExit() == PH_TRUE ? JNI_TRUE : JNI_FALSE;
}

/*
* Class:     photonApi_Ph
* Method:    phCreateEngine
* Signature: (LphotonApi/LongRef;I)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phCreateEngine
(JNIEnv* env, jclass thiz, jobject out_LongRef_engineId, jint numRenderThreads)
{
	ph::JLongRef jEngineId(out_LongRef_engineId, env);
	PHuint64     engineId;

	phCreateEngine(&engineId, static_cast<PHuint32>(numRenderThreads));

	jEngineId.setValue(static_cast<PHint64>(engineId));
}

/*
* Class:     photonApi_Ph
* Method:    phEnterCommand
* Signature: (JLjava/lang/String;)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phEnterCommand
(JNIEnv* env, jclass thiz, jlong engineId, jstring commandFragment)
{
	const char* commandFragmentString = env->GetStringUTFChars(commandFragment, JNI_FALSE);
	phEnterCommand(static_cast<PHuint64>(engineId), commandFragmentString);
	env->ReleaseStringUTFChars(commandFragment, commandFragmentString);
}

/*
* Class:     photonApi_Ph
* Method:    phRender
* Signature: (J)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phRender
(JNIEnv* env, jclass thiz, jlong engineId)
{
	phRender(static_cast<PHuint64>(engineId));
}

/*
* Class:     photonApi_Ph
* Method:    phDevelopFilm
* Signature: (JJ)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phDevelopFilm
(JNIEnv* env, jclass thiz, jlong engineId, jlong frameId)
{
	phDevelopFilm(static_cast<PHuint64>(engineId), static_cast<PHuint64>(frameId));
}

/*
* Class:     photonApi_Ph
* Method:    phGetFilmDimension
* Signature: (JLphotonApi/IntRef;LphotonApi/IntRef;)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phGetFilmDimension
(JNIEnv* env, jclass thiz, jlong engineId, jobject out_IntRef_widthPx, jobject out_IntRef_heightPx)
{
	PHuint32 widthPx, heightPx;
	phGetFilmDimension(static_cast<PHuint64>(engineId), &widthPx, &heightPx);

	ph::JIntRef jWidthPx(out_IntRef_widthPx, env);
	ph::JIntRef jHeightPx(out_IntRef_heightPx, env);
	jWidthPx.setValue(static_cast<PHint32>(widthPx));
	jHeightPx.setValue(static_cast<PHint32>(heightPx));
}

/*
* Class:     photonApi_Ph
* Method:    phDeleteEngine
* Signature: (J)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phDeleteEngine
(JNIEnv* env, jclass thiz, jlong engineId)
{
	phDeleteEngine(static_cast<PHuint64>(engineId));
}

/*
* Class:     photonApi_Ph
* Method:    phCreateFrame
* Signature: (LphotonApi/LongRef;II)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phCreateFrame
(JNIEnv* env, jclass thiz, jobject out_LongRef_frameId, jint widthPx, jint heightPx)
{
	PHuint64 frameId;
	phCreateFrame(&frameId, static_cast<PHuint32>(widthPx), static_cast<PHuint32>(heightPx));

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
* Method:    phGetFrameRgbData
* Signature: (JLphotonApi/FloatArrayRef;)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phGetFrameRgbData
(JNIEnv* env, jclass thiz, jlong frameId, jobject out_FloatArrayRef_rgbData)
{
	const PHfloat32* rgbData;
	PHuint32         widthPx;
	PHuint32         heightPx;
	const PHuint32   numComponents = 3;
	phGetFrameRgbData(static_cast<PHuint64>(frameId), &rgbData);
	phGetFrameDimension(static_cast<PHuint64>(frameId), &widthPx, &heightPx);

	const jsize numFloats = static_cast<jsize>(widthPx * heightPx * numComponents);

	jclass      class_FloatArrayRef = env->GetObjectClass(out_FloatArrayRef_rgbData);
	jfieldID    field_m_value       = env->GetFieldID(class_FloatArrayRef, "m_value", "[F");
	jfloatArray object_float_array  = env->NewFloatArray(numFloats);
	env->SetFloatArrayRegion(object_float_array, 0, numFloats, static_cast<const jfloat*>(rgbData));
	env->SetObjectField(out_FloatArrayRef_rgbData, field_m_value, object_float_array);
}

/*
* Class:     photonApi_Ph
* Method:    phQueryRendererPercentageProgress
* Signature: (JLphotonApi/FloatRef;)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phQueryRendererPercentageProgress
(JNIEnv* env, jclass thiz, jlong engineId, jobject out_FloatRef_progress)
{
	PHfloat32 progress;
	phQueryRendererPercentageProgress(static_cast<PHuint64>(engineId), &progress);
	ph::JFloatRef jProgress(out_FloatRef_progress, env);
	jProgress.setValue(progress);
}

/*
* Class:     photonApi_Ph
* Method:    phQueryRendererSampleFrequency
* Signature: (JLphotonApi/FloatRef;)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phQueryRendererSampleFrequency
(JNIEnv* env, jclass thiz, jlong engineId, jobject out_FloatRef_frequency)
{
	PHfloat32 frequency;
	phQueryRendererSampleFrequency(static_cast<PHuint64>(engineId), &frequency);
	ph::JFloatRef jFrequency(out_FloatRef_frequency, env);
	jFrequency.setValue(frequency);
}