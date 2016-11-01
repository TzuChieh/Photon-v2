#pragma once

#include "ph_core.h"

#include <jni.h>

namespace ph
{

class JIntRef final
{
public:
	// JNI methods like FindClass(), GetMethodID(), GetFieldID() are expensive operation that are guaranteed to generate 
	// the same result over the life of the JVM. Since these operations are time consuming, it is wise to retrieve 
	// and store the results here to be reused later on in the native side. This call does all these for the current
	// class.
	static void initNativeJavaClass(JNIEnv* env);

public:
	JIntRef(const jobject javaObject, JNIEnv* const env);

	PHint32 getValue() const;
	void setValue(const PHint32 value);

private:
	const jobject m_jobject;
	JNIEnv* const m_env;
	
private:
	static jclass   jclass_JIntRef;
	static jfieldID jfieldID_m_value;
};

}// end namespace ph