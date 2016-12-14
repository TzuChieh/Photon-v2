#include "photonApi_Ph.h"
#include "JIntRef.h"
#include "JLongRef.h"
#include "JFloatRef.h"

#include "ph_test.h"
#include "ph_core.h"
#include "ph_query.h"

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
* Signature: (LphotonApi/LongRef;II)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phCreateRenderer
(JNIEnv* env, jclass thiz, jobject out_LongRef_rendererId, jint rendererType, jint numThreads)
{
	ph::JLongRef jRendererId(out_LongRef_rendererId, env);
	PHuint64 rendererId;

	switch(rendererType)
	{
	case photonApi_Ph_PH_PREVIEW_RENDERER_TYPE:
		phCreateRenderer(&rendererId, PH_PREVIEW_RENDERER_TYPE, static_cast<PHuint32>(numThreads));
		break;

	case photonApi_Ph_PH_IMPORTANCE_RENDERER_TYPE:
		phCreateRenderer(&rendererId, PH_IMPORTANCE_RENDERER_TYPE, static_cast<PHuint32>(numThreads));
		break;

	case photonApi_Ph_PH_MT_IMPORTANCE_RENDERER_TYPE:
		phCreateRenderer(&rendererId, photonApi_Ph_PH_MT_IMPORTANCE_RENDERER_TYPE, static_cast<PHuint32>(numThreads));
		break;

	default:
		phCreateRenderer(&rendererId, static_cast<PHint32>(rendererType), static_cast<PHuint32>(numThreads));
	}

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
(JNIEnv* env, jclass thiz, jobject out_LongRef_frameId, jint frameType, jint frameWidthPx, jint frameHeightPx)
{
	PHuint64 frameId;
	switch(frameType)
	{
	case photonApi_Ph_PH_HDR_FRAME_TYPE:
		phCreateFrame(&frameId, PH_HDR_FRAME_TYPE, frameWidthPx, frameHeightPx);
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
* Method:    phCreateFilm
* Signature: (LphotonApi/LongRef;II)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phCreateFilm
(JNIEnv* env, jclass thiz, jobject out_LongRef_filmId, jint filmWidthPx, jint filmHeightPx)
{
	ph::JLongRef jFilmId(out_LongRef_filmId, env);

	PHuint64 filmId;
	phCreateFilm(&filmId, filmWidthPx, filmHeightPx);

	jFilmId.setValue(static_cast<PHint64>(filmId));
}

/*
* Class:     photonApi_Ph
* Method:    phDeleteFilm
* Signature: (J)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phDeleteFilm
(JNIEnv* env, jclass thiz, jlong filmId)
{
	phDeleteFrame(static_cast<PHuint64>(filmId));
}

/*
* Class:     photonApi_Ph
* Method:    phSetCameraFilm
* Signature: (JJ)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phSetCameraFilm
(JNIEnv* env, jclass thiz, jlong cameraId, jlong filmId)
{
	phSetCameraFilm(static_cast<PHuint64>(cameraId), static_cast<PHuint64>(filmId));
}

/*
* Class:     photonApi_Ph
* Method:    phCreateWorld
* Signature: (LphotonApi/LongRef;)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phCreateWorld
(JNIEnv* env, jclass thiz, jobject out_LongRef_worldId)
{
	ph::JLongRef jWorldId(out_LongRef_worldId, env);

	PHuint64 worldId;
	phCreateWorld(&worldId);

	jWorldId.setValue(static_cast<PHint64>(worldId));
}

/*
* Class:     photonApi_Ph
* Method:    phDeleteWorld
* Signature: (J)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phDeleteWorld
(JNIEnv* env, jclass thiz, jlong worldId)
{
	phDeleteWorld(static_cast<PHuint64>(worldId));
}

/*
* Class:     photonApi_Ph
* Method:    phCreateCamera
* Signature: (LphotonApi/LongRef;III)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phCreateCamera
(JNIEnv* env, jclass thiz, jobject out_LongRef_cameraId, jint cameraType)
{
	PHuint64 cameraId;
	switch(cameraType)
	{
	case photonApi_Ph_PH_DEFAULT_CAMERA_TYPE:
		phCreateCamera(&cameraId, PH_DEFAULT_CAMERA_TYPE);
		break;

	default:
		std::cerr << "unknown camera type in Java_photonApi_Ph_phCreateCamera()" << std::endl;
	}

	ph::JLongRef jCameraId(out_LongRef_cameraId, env);
	jCameraId.setValue(static_cast<PHint64>(cameraId));
}

/*
* Class:     photonApi_Ph
* Method:    phDeleteCamera
* Signature: (J)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phDeleteCamera
(JNIEnv* env, jclass thiz, jlong cameraId)
{
	phDeleteCamera(static_cast<PHuint64>(cameraId));
}

/*
* Class:     photonApi_Ph
* Method:    phLoadTestScene
* Signature: (J)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phLoadTestScene
(JNIEnv* env, jclass thiz, jlong worldId)
{
	phLoadTestScene(static_cast<PHuint64>(worldId));
}

/*
* Class:     photonApi_Ph
* Method:    phRender
* Signature: (JJJ)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phRender
(JNIEnv* env, jclass thiz, jlong rendererId, jlong worldId, jlong cameraId)
{
	phRender(static_cast<PHuint64>(rendererId), static_cast<PHuint64>(worldId), static_cast<PHuint64>(cameraId));
}

/*
* Class:     photonApi_Ph
* Method:    phCreateSampleGenerator
* Signature: (LphotonApi/LongRef;)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phCreateSampleGenerator
(JNIEnv* env, jclass thiz, jobject out_LongRef_sampleGeneratorId, jint sampleGeneratorType, jint sppBudget)
{
	PHuint64 sampleGeneratorId;
	switch(sampleGeneratorType)
	{
	case photonApi_Ph_PH_PIXEL_JITTER_SAMPLE_GENERATOR_TYPE:
		phCreateSampleGenerator(&sampleGeneratorId, PH_PIXEL_JITTER_SAMPLE_GENERATOR_TYPE, static_cast<PHuint32>(sppBudget));
		break;

	default:
		std::cerr << "unknown camera type in Java_photonApi_Ph_phCreateSampleGenerator()" << std::endl;
	}

	ph::JLongRef jSampleGeneratorId(out_LongRef_sampleGeneratorId, env);
	jSampleGeneratorId.setValue(static_cast<PHint64>(sampleGeneratorId));
}

/*
* Class:     photonApi_Ph
* Method:    phDeleteSampleGenerator
* Signature: (J)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phDeleteSampleGenerator
(JNIEnv* env, jclass thiz, jlong sampleGeneratorId)
{
	phDeleteSampleGenerator(static_cast<PHuint64>(sampleGeneratorId));
}

/*
* Class:     photonApi_Ph
* Method:    phSetRendererSampleGenerator
* Signature: (JJ)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phSetRendererSampleGenerator
(JNIEnv* env, jclass thiz, jlong rendererId, jlong sampleGeneratorId)
{
	phSetRendererSampleGenerator(static_cast<PHuint64>(rendererId), static_cast<PHuint64>(sampleGeneratorId));
}

/*
* Class:     photonApi_Ph
* Method:    phCookWorld
* Signature: (J)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phCookWorld
(JNIEnv* env, jclass thiz, jlong worldId)
{
	phCookWorld(static_cast<PHuint64>(worldId));
}

/*
* Class:     photonApi_Ph
* Method:    phDevelopFilm
* Signature: (JJ)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phDevelopFilm
(JNIEnv* env, jclass thiz, jlong filmId, jlong frameId)
{
	phDevelopFilm(static_cast<PHuint64>(filmId), static_cast<PHuint64>(frameId));
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
* Method:    phSetCameraPosition
* Signature: (JFFF)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phSetCameraPosition
(JNIEnv* env, jclass thiz, jlong cameraId, jfloat x, jfloat y, jfloat z)
{
	phSetCameraPosition(static_cast<PHuint64>(cameraId), x, y, z);
}

/*
* Class:     photonApi_Ph
* Method:    phSetCameraDirection
* Signature: (JFFF)V
*/
JNIEXPORT void JNICALL Java_photonApi_Ph_phSetCameraDirection
(JNIEnv* env, jclass thiz, jlong cameraId, jfloat x, jfloat y, jfloat z)
{
	phSetCameraDirection(static_cast<PHuint64>(cameraId), x, y, z);
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