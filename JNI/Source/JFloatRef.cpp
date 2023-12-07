#include "JFloatRef.h"
#include "java_type_signature.h"

#include <iostream>

#define JAVA_FLOATREF_CLASS_NAME "photonApi/FloatRef"
#define JAVA_FLOATREF_VALUE_NAME "m_value"

namespace ph
{

JFloatRef::JFloatRef(const jobject javaObject, JNIEnv* const env) :
	JObject<JFloatRef>(javaObject, env)
{
	static bool isCached = false;
	if(!isCached)
	{
		cacheJavaClass(JAVA_FLOATREF_CLASS_NAME, env);
		cacheJavaFieldId(JAVA_FLOATREF_VALUE_NAME, JAVA_FLOAT_SIGNATURE, env);
		isCached = true;
	}
}

PhFloat32 JFloatRef::getValue() const
{
	return static_cast<PhFloat32>(m_env->GetFloatField(m_jobject, getFieldId(JAVA_FLOATREF_VALUE_NAME)));
}

void JFloatRef::setValue(const PhFloat32 value)
{
	m_env->SetFloatField(m_jobject, getFieldId(JAVA_FLOATREF_VALUE_NAME), static_cast<jfloat>(value));
}

}// end namespace ph