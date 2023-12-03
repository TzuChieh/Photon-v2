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

	PHint64 getValue() const;
	void setValue(const PHint64 value);
};

}// end namespace ph