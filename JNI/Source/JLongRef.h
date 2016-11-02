#pragma once

#include "ph_core.h"
#include "JObject.h"

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