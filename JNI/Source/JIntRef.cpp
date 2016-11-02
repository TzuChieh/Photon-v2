#include "JIntRef.h"
#include "java_type_signature.h"

#include <iostream>

#define JAVA_INTREF_CLASS_NAME "photonApi/IntRef"
#define JAVA_INTREF_VALUE_NAME "m_value"

namespace ph
{

JIntRef::JIntRef(const jobject javaObject, JNIEnv* const env) :
	JObject<JIntRef>(javaObject, env)
{
	static bool isCached = false;
	if(!isCached)
	{
		cacheJavaClass(JAVA_INTREF_CLASS_NAME, env);
		cacheJavaFieldId(JAVA_INTREF_VALUE_NAME, JAVA_INT_SIGNATURE, env);
	}
}

PHint32 JIntRef::getValue() const
{
	return static_cast<PHint32>(m_env->GetIntField(m_jobject, getFieldId(JAVA_INTREF_VALUE_NAME)));
}

void JIntRef::setValue(const PHint32 value)
{
	m_env->SetIntField(m_jobject, getFieldId(JAVA_INTREF_VALUE_NAME), static_cast<jint>(value));
}

}// end namespace ph