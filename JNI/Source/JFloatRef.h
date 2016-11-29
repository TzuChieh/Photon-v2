#pragma once

#include "ph_core.h"
#include "JObject.h"

namespace ph
{

class JFloatRef final : public JObject<JFloatRef>
{
public:
	JFloatRef(const jobject javaObject, JNIEnv* const env);

	PHfloat32 getValue() const;
	void setValue(const PHfloat32 value);
};

}// end namespace ph