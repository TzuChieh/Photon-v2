#include "JLongRef.h"
#include "java_type_signature.h"

#define JAVA_LONGREF_CLASS_NAME "photonApi/LongRef"
#define JAVA_LONGREF_VALUE_NAME "m_value"

namespace ph
{

JLongRef::JLongRef(const jobject javaObject, JNIEnv* const env) :
	JObject<JLongRef>(javaObject, env)
{
	static bool isCached = false;
	if(!isCached)
	{
		cacheJavaClass(JAVA_LONGREF_CLASS_NAME, env);
		cacheJavaFieldId(JAVA_LONGREF_VALUE_NAME, JAVA_LONG_SIGNATURE, env);
		isCached = true;
	}
}

PhInt64 JLongRef::getValue() const
{
	return static_cast<PhInt64>(m_env->GetLongField(m_jobject, getFieldId(JAVA_LONGREF_VALUE_NAME)));
}

void JLongRef::setValue(const PhInt64 value)
{
	m_env->SetLongField(m_jobject, getFieldId(JAVA_LONGREF_VALUE_NAME), static_cast<jlong>(value));
}

}// end namespace ph