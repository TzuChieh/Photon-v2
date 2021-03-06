/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class photonApi_Ph */

#ifndef _Included_photonApi_Ph
#define _Included_photonApi_Ph
#ifdef __cplusplus
extern "C" {
#endif
#undef photonApi_Ph_FILM_REGION_STATUS_INVALID
#define photonApi_Ph_FILM_REGION_STATUS_INVALID 0L
#undef photonApi_Ph_FILM_REGION_STATUS_UPDATING
#define photonApi_Ph_FILM_REGION_STATUS_UPDATING 1L
#undef photonApi_Ph_FILM_REGION_STATUS_FINISHED
#define photonApi_Ph_FILM_REGION_STATUS_FINISHED 2L
/*
 * Class:     photonApi_Ph
 * Method:    phInit
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_photonApi_Ph_phInit
  (JNIEnv *, jclass);

/*
 * Class:     photonApi_Ph
 * Method:    phExit
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_photonApi_Ph_phExit
  (JNIEnv *, jclass);

/*
 * Class:     photonApi_Ph
 * Method:    phCreateEngine
 * Signature: (LphotonApi/LongRef;I)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phCreateEngine
  (JNIEnv *, jclass, jobject, jint);

/*
 * Class:     photonApi_Ph
 * Method:    phSetNumRenderThreads
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phSetNumRenderThreads
  (JNIEnv *, jclass, jlong, jint);

/*
 * Class:     photonApi_Ph
 * Method:    phEnterCommand
 * Signature: (JLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phEnterCommand
  (JNIEnv *, jclass, jlong, jstring);

/*
 * Class:     photonApi_Ph
 * Method:    phUpdate
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phUpdate
  (JNIEnv *, jclass, jlong);

/*
 * Class:     photonApi_Ph
 * Method:    phRender
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phRender
  (JNIEnv *, jclass, jlong);

/*
 * Class:     photonApi_Ph
 * Method:    phAquireFrame
 * Signature: (JIJ)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phAquireFrame
  (JNIEnv *, jclass, jlong, jint, jlong);

/*
 * Class:     photonApi_Ph
 * Method:    phGetRenderDimension
 * Signature: (JLphotonApi/IntRef;LphotonApi/IntRef;)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phGetRenderDimension
  (JNIEnv *, jclass, jlong, jobject, jobject);

/*
 * Class:     photonApi_Ph
 * Method:    phGetObservableRenderData
 * Signature: (JLphotonApi/ObservableRenderData;)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phGetObservableRenderData
  (JNIEnv *, jclass, jlong, jobject);

/*
 * Class:     photonApi_Ph
 * Method:    phDeleteEngine
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phDeleteEngine
  (JNIEnv *, jclass, jlong);

/*
 * Class:     photonApi_Ph
 * Method:    phSetWorkingDirectory
 * Signature: (JLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phSetWorkingDirectory
  (JNIEnv *, jclass, jlong, jstring);

/*
 * Class:     photonApi_Ph
 * Method:    phCreateFrame
 * Signature: (LphotonApi/LongRef;II)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phCreateFrame
  (JNIEnv *, jclass, jobject, jint, jint);

/*
 * Class:     photonApi_Ph
 * Method:    phCopyFrameRgbData
 * Signature: (JLphotonApi/FloatArrayRef;)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phCopyFrameRgbData__JLphotonApi_FloatArrayRef_2
  (JNIEnv *, jclass, jlong, jobject);

/*
 * Class:     photonApi_Ph
 * Method:    phCopyFrameRgbData
 * Signature: (JIIIILphotonApi/FloatArrayRef;)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phCopyFrameRgbData__JIIIILphotonApi_FloatArrayRef_2
  (JNIEnv *, jclass, jlong, jint, jint, jint, jint, jobject);

/*
 * Class:     photonApi_Ph
 * Method:    phDeleteFrame
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phDeleteFrame
  (JNIEnv *, jclass, jlong);

/*
 * Class:     photonApi_Ph
 * Method:    phAsyncGetRendererStatistics
 * Signature: (JLphotonApi/FloatRef;LphotonApi/FloatRef;)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phAsyncGetRendererStatistics
  (JNIEnv *, jclass, jlong, jobject, jobject);

/*
 * Class:     photonApi_Ph
 * Method:    phAsyncPollUpdatedFrameRegion
 * Signature: (JLphotonApi/IntRef;LphotonApi/IntRef;LphotonApi/IntRef;LphotonApi/IntRef;)I
 */
JNIEXPORT jint JNICALL Java_photonApi_Ph_phAsyncPollUpdatedFrameRegion
  (JNIEnv *, jclass, jlong, jobject, jobject, jobject, jobject);

/*
 * Class:     photonApi_Ph
 * Method:    phAsyncPeekFrame
 * Signature: (JIIIIIJ)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phAsyncPeekFrame
  (JNIEnv *, jclass, jlong, jint, jint, jint, jint, jint, jlong);

/*
 * Class:     photonApi_Ph
 * Method:    phAsyncGetRendererState
 * Signature: (JLphotonApi/RenderState;)V
 */
JNIEXPORT void JNICALL Java_photonApi_Ph_phAsyncGetRendererState
  (JNIEnv *, jclass, jlong, jobject);

#ifdef __cplusplus
}
#endif
#endif
