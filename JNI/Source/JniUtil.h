#pragma once

#include <jni.h>

namespace ph
{

class JniUtil final
{
public:
	static inline bool isJavaNull(JNIEnv* env, jobject obj)
	{
		return env->IsSameObject(obj, NULL) == JNI_TRUE;
	}
};

}// end namespace ph