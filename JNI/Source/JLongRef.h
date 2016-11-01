#pragma once

#include "ph_core.h"

#include <jni.h>

namespace ph
{

class JLongRef final
{
public:
	// JNI methods like FindClass(), GetMethodID(), GetFieldID() are expensive operation that are guaranteed to generate 
	// the same result over the life of the JVM. Since these operations are time consuming, it is wise to retrieve 
	// and store the results here to be reused later on in the native side. This call does all these for the current
	// class.
	static void initNativeJavaClass(JNIEnv* env);

public:
	JLongRef(const jobject javaObject, JNIEnv* const env);

	PHint64 getValue() const;
	void setValue(const PHint64 value);

private:
	const jobject m_jobject;
	JNIEnv* const m_env;
	
private:
	static jclass   jclass_LongRef;
	static jfieldID jfieldID_m_value;
};

}// end namespace ph