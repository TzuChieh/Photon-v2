#include "JLongRef.h"
#include "java_type_signature.h"

#define JAVA_LONGREF_CLASS_NAME "photonApi/LongRef"
#define JAVA_LONGREF_VALUE_NAME "m_value"

namespace ph
{

jclass   JLongRef::jclass_LongRef;
jfieldID JLongRef::jfieldID_m_value;

void JLongRef::initNativeJavaClass(JNIEnv* env)
{
	jclass_LongRef   = env->FindClass(JAVA_LONGREF_CLASS_NAME);
	jfieldID_m_value = env->GetFieldID(jclass_LongRef, JAVA_LONGREF_VALUE_NAME, JAVA_LONG_SIGNATURE);
}

JLongRef::JLongRef(const jobject javaObject, JNIEnv* const env) : 
	m_jobject(javaObject), m_env(env)
{

}

PHint64 JLongRef::getValue() const
{
	return static_cast<PHint64>(m_env->GetLongField(m_jobject, jfieldID_m_value));
}

void JLongRef::setValue(const PHint64 value)
{
	m_env->SetLongField(m_jobject, jfieldID_m_value, static_cast<jlong>(value));
}

}// end namespace ph