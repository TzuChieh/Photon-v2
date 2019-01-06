#include "photonApi_Ph.h"
#include "JIntRef.h"
#include "JLongRef.h"
#include "JFloatRef.h"
#include "JniUtil.h"
#include "JniHelper.h"
#include "java_type_signature.h"

#include <ph_core.h>

#include <iostream>
#include <array>

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
* Method:    phSetNumRenderThreads
* Signature: (JI)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phSetNumRenderThreads
(JNIEnv* env, jclass thiz, jlong engineId, jint numRenderThreads)
{
	phSetNumRenderThreads(static_cast<PHuint64>(engineId), static_cast<PHuint32>(numRenderThreads));
}

/*
* Class:     photonApi_Ph
* Method:    phEnterCommand
* Signature: (JLjava/lang/String;)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phEnterCommand
(JNIEnv* env, jclass thiz, jlong engineId, jstring commandFragment)
{
	const char* commandFragmentString = env->GetStringUTFChars(commandFragment, nullptr);
	if(commandFragmentString)
	{
		phEnterCommand(static_cast<PHuint64>(engineId), commandFragmentString);
		
	}
	else
	{
		if(env->ExceptionCheck())
		{
			env->ExceptionDescribe();
			env->ExceptionClear();
		}

		std::cerr << "warning: " << "GetStringUTFChars() returns null for engine " << std::to_string(engineId) << std::endl;
	}
	env->ReleaseStringUTFChars(commandFragment, commandFragmentString);
}

/*
* Class:     photonApi_Ph
* Method:    phUpdate
* Signature: (J)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phUpdate
(JNIEnv* env, jclass thiz, jlong engineId)
{
	phUpdate(static_cast<PHuint64>(engineId));
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
(JNIEnv* env, jclass thiz, jlong engineId, jlong frameId, jint attribute)
{
	phDevelopFilm(static_cast<PHuint64>(engineId), static_cast<PHuint64>(frameId), ph::JniHelper::toCAttribute(attribute));
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
* Method:    phSetWorkingDirectory
* Signature: (JLjava/lang/String;)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phSetWorkingDirectory
(JNIEnv* env, jclass clazz, jlong engineId, jstring workingDirectory)
{
	static_assert(sizeof(PHchar) == sizeof(char), 
	              "character size mismatch between Photon's char and C++'s.");

	const char* workingDirectoryString = env->GetStringUTFChars(workingDirectory, nullptr);
	if(workingDirectoryString)
	{
		phSetWorkingDirectory(static_cast<PHuint64>(engineId), workingDirectoryString);

	}
	else
	{
		if(env->ExceptionCheck())
		{
			env->ExceptionDescribe();
			env->ExceptionClear();
		}

		std::cerr << "warning: " << "GetStringUTFChars() returns null for engine " << std::to_string(engineId) << std::endl;
	}
	env->ReleaseStringUTFChars(workingDirectory, workingDirectoryString);
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
* Method:    phCopyFrameRgbData
* Signature: (JLphotonApi/FloatArrayRef;)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phCopyFrameRgbData__JLphotonApi_FloatArrayRef_2
(JNIEnv* env, jclass thiz, jlong frameId, jobject out_FloatArrayRef_rgbData)
{
	PHuint32 widthPx;
	PHuint32 heightPx;
	phGetFrameDimension(static_cast<PHuint64>(frameId), &widthPx, &heightPx);

	Java_photonApi_Ph_phCopyFrameRgbData__JIIIILphotonApi_FloatArrayRef_2
	(
		env, thiz, frameId, 
		0, 0, static_cast<jint>(widthPx), static_cast<jint>(heightPx), 
		out_FloatArrayRef_rgbData
	);
}

/*
* Class:     photonApi_Ph
* Method:    phCopyFrameRgbData
* Signature: (JIIIILphotonApi/FloatArrayRef;)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phCopyFrameRgbData__JIIIILphotonApi_FloatArrayRef_2
(JNIEnv* env, jclass thiz, jlong frameId, 
                           jint xPx, jint yPx, jint wPx, jint hPx, 
                           jobject out_FloatArrayRef_rgbData)
{
	const PHfloat32* rgbData;
	PHuint32         widthPx;
	PHuint32         heightPx;
	const jsize      numComp = 3;
	phGetFrameRgbData(static_cast<PHuint64>(frameId), &rgbData);
	phGetFrameDimension(static_cast<PHuint64>(frameId), &widthPx, &heightPx);

	const jsize numFloats = static_cast<jsize>(wPx * hPx * numComp);
	jfloatArray object_float_array = env->NewFloatArray(numFloats);
	for(jint y = yPx; y < yPx + hPx; y++)
	{
		const std::size_t dataStartIndex = (static_cast<std::size_t>(y) * widthPx + 
		                                    static_cast<std::size_t>(xPx)) * static_cast<std::size_t>(numComp);

		const jsize arrayOffset = static_cast<jsize>(y - yPx) * static_cast<jsize>(wPx) * numComp;
		const jsize length      = static_cast<jsize>(wPx) * numComp;

		env->SetFloatArrayRegion(object_float_array, 
		                         arrayOffset, length,
		                         static_cast<const jfloat*>(rgbData + dataStartIndex));
	}

	jclass   class_FloatArrayRef = env->GetObjectClass(out_FloatArrayRef_rgbData);
	jfieldID field_m_value       = env->GetFieldID(class_FloatArrayRef, "m_value", JAVA_FLOAT_ARRAY_SIGNATURE);
	env->SetObjectField(out_FloatArrayRef_rgbData, field_m_value, object_float_array);
}

/*
* Class:     photonApi_Ph
* Method:    phAsyncGetRendererStatistics
* Signature: (JLphotonApi/FloatRef;LphotonApi/FloatRef;)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phAsyncGetRendererStatistics
(JNIEnv* env, jclass thiz, jlong engineId, 
                           jobject out_FloatRef_percentageProgress, 
                           jobject out_FloatRef_samplesPerSecond)
{
	PHfloat32 progress, frequency;
	phAsyncGetRendererStatistics(static_cast<PHuint64>(engineId), &progress, &frequency);

	ph::JFloatRef jProgress(out_FloatRef_percentageProgress, env);
	ph::JFloatRef jFrequency(out_FloatRef_samplesPerSecond, env);
	jProgress.setValue(progress);
	jFrequency.setValue(frequency);
}

/*
* Class:     photonApi_Ph
* Method:    phAsyncPollUpdatedFilmRegion
* Signature: (JLphotonApi/IntRef;LphotonApi/IntRef;LphotonApi/IntRef;LphotonApi/IntRef;)I
*/
JNIEXPORT jint JNICALL Java_photonApi_Ph_phAsyncPollUpdatedFilmRegion
(JNIEnv* env, jclass thiz, jlong engineId, 
                           jobject out_IntRef_xPx, jobject out_IntRef_yPx, 
                           jobject out_IntRef_wPx, jobject out_IntRef_hPx)
{
	PHuint32 xPx, yPx, wPx, hPx;
	const int status = phAsyncPollUpdatedFilmRegion(static_cast<PHuint64>(engineId), 
	                                                &xPx, &yPx, &wPx, &hPx);

	ph::JIntRef jXpx(out_IntRef_xPx, env);
	ph::JIntRef jYpx(out_IntRef_yPx, env);
	ph::JIntRef jWpx(out_IntRef_wPx, env);
	ph::JIntRef jHpx(out_IntRef_hPx, env);
	jXpx.setValue(static_cast<PHint32>(xPx));
	jYpx.setValue(static_cast<PHint32>(yPx));
	jWpx.setValue(static_cast<PHint32>(wPx));
	jHpx.setValue(static_cast<PHint32>(hPx));

	switch(status)
	{
	case PH_FILM_REGION_STATUS_UPDATING: return photonApi_Ph_FILM_REGION_STATUS_UPDATING;
	case PH_FILM_REGION_STATUS_FINISHED: return photonApi_Ph_FILM_REGION_STATUS_FINISHED;
	}

	return photonApi_Ph_FILM_REGION_STATUS_INVALID;
}

/*
* Class:     photonApi_Ph
* Method:    phAsyncDevelopFilmRegion
* Signature: (JJIIII)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phAsyncDevelopFilmRegion
(JNIEnv* env, jclass thiz, jlong engineId, jlong frameId, 
                           jint xPx, jint yPx, jint wPx, jint hPx, jint attribute)
{
	phAsyncDevelopFilmRegion(static_cast<PHuint64>(engineId), 
	                         static_cast<PHuint64>(frameId),
	                         static_cast<PHuint32>(xPx),
	                         static_cast<PHuint32>(yPx),
	                         static_cast<PHuint32>(wPx),
	                         static_cast<PHuint32>(hPx), ph::JniHelper::toCAttribute(attribute));
}

/*
 * Class:     photonApi_Ph
 * Method:    phGetRenderStateName
 * Signature: (JIILphotonApi/StringRef;)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phGetRenderStateName
(JNIEnv* env, jclass thiz, jlong engineId, jint type, jint index, jobject out_StringRef_name)
{
	enum PH_ERenderStateType stateType;
	if(type == photonApi_Ph_RENDER_STATE_INTEGER)
	{
		stateType = INTEGER;
	}
	else if(type == photonApi_Ph_RENDER_STATE_REAL)
	{
		stateType = REAL;
	}

	std::array<PHchar, 128> nameBuffer;
	phGetRenderStateName(
		static_cast<PHuint64>(engineId), 
		stateType, 
		static_cast<PHuint32>(index), 
		nameBuffer.data(), 
		static_cast<PHuint32>(nameBuffer.size()));

	jclass   class_StringRef = env->GetObjectClass(out_StringRef_name);
	jfieldID field_m_value   = env->GetFieldID(class_StringRef, "m_value", JAVA_STRING_SIGNATURE);
	jstring  jNameString     = env->NewStringUTF(nameBuffer.data());
	env->SetObjectField(out_StringRef_name, field_m_value, jNameString);
}

/*
 * Class:     photonApi_Ph
 * Method:    phAsyncGetRendererState
 * Signature: (JLphotonApi/RenderState;)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phAsyncGetRendererState
(JNIEnv* env, jclass thiz, jlong engineId, jobject out_RenderState_state)
{
	struct PH_RenderState state;
	phAsyncGetRendererState(static_cast<PHuint64>(engineId), &state);

	jlongArray jLongArray = env->NewLongArray(PH_NUM_RENDER_STATE_INTEGERS);
	for(std::size_t i = 0; i < PH_NUM_RENDER_STATE_INTEGERS; ++i)
	{
		const jlong value = static_cast<jlong>(state.integers[i]);
		env->SetLongArrayRegion(jLongArray, static_cast<jsize>(i), 1, &value);
	}

	jfloatArray jFloatArray = env->NewFloatArray(PH_NUM_RENDER_STATE_REALS);
	for(std::size_t i = 0; i < PH_NUM_RENDER_STATE_REALS; ++i)
	{
		const jfloat value = static_cast<jfloat>(state.reals[i]);
		env->SetFloatArrayRegion(jFloatArray, static_cast<jsize>(i), 1, &value);
	}

	jclass   class_RenderState   = env->GetObjectClass(out_RenderState_state);
	jfieldID field_integerStates = env->GetFieldID(class_RenderState, "integerStates", JAVA_LONG_ARRAY_SIGNATURE);
	jfieldID field_realStates    = env->GetFieldID(class_RenderState, "realStates",    JAVA_FLOAT_ARRAY_SIGNATURE);
	env->SetObjectField(out_RenderState_state, field_integerStates, jLongArray);
	env->SetObjectField(out_RenderState_state, field_realStates,    jFloatArray);
}