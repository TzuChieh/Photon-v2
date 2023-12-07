#pragma once

#include "JObject.h"

#include <ph_c_core.h>

namespace ph
{

class JIntRef final : public JObject<JIntRef>
{
public:
	JIntRef(const jobject javaObject, JNIEnv* const env);

	PhInt32 getValue() const;
	void setValue(const PhInt32 value);
};

}// end namespace ph