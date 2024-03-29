#include "photonApi_Ph.h"
#include "JIntRef.h"
#include "JLongRef.h"
#include "JFloatRef.h"
#include "JniUtil.h"
#include "java_type_signature.h"

#include <ph_c_core.h>

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
	PhUInt64     engineId;

	phCreateEngine(&engineId, static_cast<PhUInt32>(numRenderThreads));

	jEngineId.setValue(static_cast<PhUInt64>(engineId));
}

/*
* Class:     photonApi_Ph
* Method:    phSetNumRenderThreads
* Signature: (JI)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phSetNumRenderThreads
(JNIEnv* env, jclass thiz, jlong engineId, jint numRenderThreads)
{
	phSetNumRenderThreads(static_cast<PhUInt64>(engineId), static_cast<PhUInt32>(numRenderThreads));
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
		phEnterCommand(static_cast<PhUInt64>(engineId), commandFragmentString);
		
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
	phUpdate(static_cast<PhUInt64>(engineId));
}

/*
* Class:     photonApi_Ph
* Method:    phRender
* Signature: (J)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phRender
(JNIEnv* env, jclass thiz, jlong engineId)
{
	phRender(static_cast<PhUInt64>(engineId));
}

/*
 * Class:     photonApi_Ph
 * Method:    phAquireFrame
 * Signature: (JIJ)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phAquireFrame
(JNIEnv* env, jclass clazz, jlong engineId, jint channelIndex, jlong frameId)
{
	phAquireFrame(
		static_cast<PhUInt64>(engineId),
		static_cast<PhUInt64>(channelIndex),
		static_cast<PhUInt64>(frameId));
}

/*
* Class:     photonApi_Ph
* Method:    phGetFilmDimension
* Signature: (JLphotonApi/IntRef;LphotonApi/IntRef;)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phGetRenderDimension
(JNIEnv* env, jclass clazz, jlong engineId, jobject out_IntRef_widthPx, jobject out_IntRef_heightPx)
{
	PhUInt32 widthPx, heightPx;
	phGetRenderDimension(static_cast<PhUInt64>(engineId), &widthPx, &heightPx);

	ph::JIntRef jWidthPx(out_IntRef_widthPx, env);
	ph::JIntRef jHeightPx(out_IntRef_heightPx, env);
	jWidthPx.setValue(static_cast<PhInt32>(widthPx));
	jHeightPx.setValue(static_cast<PhInt32>(heightPx));
}

/*
* Class:     photonApi_Ph
* Method:    phDeleteEngine
* Signature: (J)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phDeleteEngine
(JNIEnv* env, jclass thiz, jlong engineId)
{
	phDeleteEngine(static_cast<PhUInt64>(engineId));
}

/*
* Class:     photonApi_Ph
* Method:    phSetWorkingDirectory
* Signature: (JLjava/lang/String;)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phSetWorkingDirectory
(JNIEnv* env, jclass clazz, jlong engineId, jstring workingDirectory)
{
	static_assert(sizeof(PhChar) == sizeof(char), 
	              "character size mismatch between Photon's char and C++'s.");

	const char* workingDirectoryString = env->GetStringUTFChars(workingDirectory, nullptr);
	if(workingDirectoryString)
	{
		phSetWorkingDirectory(static_cast<PhUInt64>(engineId), workingDirectoryString);

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
	PhUInt64 frameId;
	phCreateFrame(&frameId, static_cast<PhUInt32>(widthPx), static_cast<PhUInt32>(heightPx));

	ph::JLongRef jFrameId(out_LongRef_frameId, env);
	jFrameId.setValue(static_cast<PhUInt64>(frameId));
}

/*
* Class:     photonApi_Ph
* Method:    phDeleteFrame
* Signature: (J)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phDeleteFrame
(JNIEnv* env, jclass thiz, jlong frameId)
{
	phDeleteFrame(static_cast<PhUInt64>(frameId));
}

/*
* Class:     photonApi_Ph
* Method:    phCopyFrameRgbData
* Signature: (JLphotonApi/FloatArrayRef;)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phCopyFrameRgbData__JLphotonApi_FloatArrayRef_2
(JNIEnv* env, jclass thiz, jlong frameId, jobject out_FloatArrayRef_rgbData)
{
	PhUInt32 widthPx;
	PhUInt32 heightPx;
	phGetFrameDimension(static_cast<PhUInt64>(frameId), &widthPx, &heightPx);

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
	const PhFloat32* rgbData;
	PhUInt32         widthPx;
	PhUInt32         heightPx;
	const jsize      numComp = 3;
	phGetFrameRgbData(static_cast<PhUInt64>(frameId), &rgbData);
	phGetFrameDimension(static_cast<PhUInt64>(frameId), &widthPx, &heightPx);

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
	PhFloat32 progress, frequency;
	phAsyncGetRenderStatistics(static_cast<PhUInt64>(engineId), &progress, &frequency);

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
JNIEXPORT jint JNICALL Java_photonApi_Ph_phAsyncPollUpdatedFrameRegion(
	JNIEnv* env, jclass clazz, 
	jlong   engineId, 
	jobject out_IntRef_xPx, 
	jobject out_IntRef_yPx, 
	jobject out_IntRef_wPx, 
	jobject out_IntRef_hPx)
{
	PhFrameRegionInfo regionInfo;
	if(!phAsyncPollUpdatedFrameRegion(static_cast<PhUInt64>(engineId), &regionInfo))
	{
		return photonApi_Ph_FILM_REGION_STATUS_INVALID;
	}

	ph::JIntRef jXpx(out_IntRef_xPx, env);
	ph::JIntRef jYpx(out_IntRef_yPx, env);
	ph::JIntRef jWpx(out_IntRef_wPx, env);
	ph::JIntRef jHpx(out_IntRef_hPx, env);
	jXpx.setValue(static_cast<PhInt32>(regionInfo.xPx));
	jYpx.setValue(static_cast<PhInt32>(regionInfo.yPx));
	jWpx.setValue(static_cast<PhInt32>(regionInfo.widthPx));
	jHpx.setValue(static_cast<PhInt32>(regionInfo.heightPx));

	switch(regionInfo.status)
	{
	case PH_FRAME_REGION_STATUS_UPDATING: return photonApi_Ph_FILM_REGION_STATUS_UPDATING;
	case PH_FRAME_REGION_STATUS_FINISHED: return photonApi_Ph_FILM_REGION_STATUS_FINISHED;
	}

	return photonApi_Ph_FILM_REGION_STATUS_INVALID;
}

/*
* Class:     photonApi_Ph
* Method:    phAsyncDevelopFilmRegion
* Signature: (JJIIII)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phAsyncPeekFrame(
	JNIEnv* env, jclass clazz, 
	jlong engineId, 
	jint  channelIndex, 
	jint  xPx, 
	jint  yPx, 
	jint  wPx, 
	jint  hPx, 
	jlong frameId)
{
	phAsyncPeekFrame(
		static_cast<PhUInt64>(engineId),
		static_cast<PhUInt64>(channelIndex),
		static_cast<PhUInt32>(xPx),
		static_cast<PhUInt32>(yPx),
		static_cast<PhUInt32>(wPx),
		static_cast<PhUInt32>(hPx),
		static_cast<PhUInt32>(frameId));
}

/*
 * Class:     photonApi_Ph
 * Method:    phGetObservableRenderData
 * Signature: (JLphotonApi/ObservableRenderData;)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phGetObservableRenderData
(JNIEnv* env, jclass thiz, jlong engineId, jobject out_ObservableRenderData_data)
{
	PHObservableRenderData data;
	phGetObservableRenderData(static_cast<PhUInt64>(engineId), &data);

	jobjectArray jLayerNamesStringArray = env->NewObjectArray(
		PH_NUM_RENDER_LAYERS, 
		env->FindClass(JAVA_STRING_SIGNATURE), 
		env->NewStringUTF(""));
	for(jsize i = 0; i < PH_NUM_RENDER_LAYERS; ++i)
	{
		env->SetObjectArrayElement(
			jLayerNamesStringArray, 
			i, 
			env->NewStringUTF(data.layers[i]));
	}

	jobjectArray jIntegerNamesStringArray = env->NewObjectArray(
		PH_NUM_RENDER_STATE_INTEGERS,
		env->FindClass(JAVA_STRING_SIGNATURE),
		env->NewStringUTF(""));
	for(jsize i = 0; i < PH_NUM_RENDER_STATE_INTEGERS; ++i)
	{
		env->SetObjectArrayElement(
			jIntegerNamesStringArray,
			i,
			env->NewStringUTF(data.integers[i]));
	}

	jobjectArray jRealNamesStringArray = env->NewObjectArray(
		PH_NUM_RENDER_STATE_REALS,
		env->FindClass(JAVA_STRING_SIGNATURE),
		env->NewStringUTF(""));
	for(jsize i = 0; i < PH_NUM_RENDER_STATE_REALS; ++i)
	{
		env->SetObjectArrayElement(
			jRealNamesStringArray,
			i,
			env->NewStringUTF(data.reals[i]));
	}

	jclass class_ObservableRenderData = env->GetObjectClass(out_ObservableRenderData_data);

	jfieldID field_layerNames   = env->GetFieldID(class_ObservableRenderData, "layerNames",   JAVA_STRING_ARRAY_SIGNATURE);
	jfieldID field_integerNames = env->GetFieldID(class_ObservableRenderData, "integerNames", JAVA_STRING_ARRAY_SIGNATURE);
	jfieldID field_realNames    = env->GetFieldID(class_ObservableRenderData, "realNames",    JAVA_STRING_ARRAY_SIGNATURE);

	env->SetObjectField(out_ObservableRenderData_data, field_layerNames,   jLayerNamesStringArray);
	env->SetObjectField(out_ObservableRenderData_data, field_integerNames, jIntegerNamesStringArray);
	env->SetObjectField(out_ObservableRenderData_data, field_realNames,    jRealNamesStringArray);
}

/*
 * Class:     photonApi_Ph
 * Method:    phAsyncGetRendererState
 * Signature: (JLphotonApi/RenderState;)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phAsyncGetRendererState
(JNIEnv* env, jclass thiz, jlong engineId, jobject out_RenderState_state)
{
	struct PHRenderState state;
	phAsyncGetRenderState(static_cast<PhUInt64>(engineId), &state);

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