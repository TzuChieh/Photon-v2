#include "JIntRef.h"
#include "java_type_signature.h"

#include <iostream>

#define JAVA_INTREF_CLASS_NAME "photonApi/IntRef"
#define JAVA_INTREF_VALUE_NAME "m_value"

namespace ph
{

jclass   JIntRef::jclass_JIntRef;
jfieldID JIntRef::jfieldID_m_value;

void JIntRef::initNativeJavaClass(JNIEnv* env)
{
	jclass localRef = env->FindClass(JAVA_INTREF_CLASS_NAME);
	jclass_JIntRef = static_cast<jclass>(env->NewGlobalRef(localRef));

	if(env->ExceptionCheck())
	{
		env->ExceptionClear();
		std::cerr << "error 1" << std::endl;
	}

	jfieldID_m_value = env->GetFieldID(jclass_JIntRef, JAVA_INTREF_VALUE_NAME, JAVA_INT_SIGNATURE);


	//env->DeleteLocalRef(tmp_point_Class);

	if(env->ExceptionCheck())
	{
		env->ExceptionClear();
		std::cerr << "error 2" << std::endl;
	}
}

JIntRef::JIntRef(const jobject javaObject, JNIEnv* const env) :
	m_jobject(javaObject), m_env(env)
{

}

PHint32 JIntRef::getValue() const
{
	return static_cast<PHint32>(m_env->GetIntField(m_jobject, jfieldID_m_value));
}

void JIntRef::setValue(const PHint32 value)
{
	m_env->SetIntField(m_jobject, jfieldID_m_value, static_cast<jint>(value));
}

}// end namespace ph