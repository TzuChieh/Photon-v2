#pragma once

#include "JObject.h"

#include <ph_c_core.h>

namespace ph
{

class JFloatRef final : public JObject<JFloatRef>
{
public:
	JFloatRef(const jobject javaObject, JNIEnv* const env);

	PhFloat32 getValue() const;
	void setValue(const PhFloat32 value);
};

}// end namespace ph