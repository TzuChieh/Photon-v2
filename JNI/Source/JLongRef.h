#pragma once

#include "JObject.h"

#include <ph_c_core.h>

#include <jni.h>

namespace ph
{

class JLongRef final : public JObject<JLongRef>
{
public:
	JLongRef(const jobject javaObject, JNIEnv* const env);

	PhInt64 getValue() const;
	void setValue(const PhInt64 value);
};

}// end namespace ph