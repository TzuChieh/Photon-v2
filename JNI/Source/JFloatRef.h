#pragma once

#include "JObject.h"

#include <ph_c_core.h>

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