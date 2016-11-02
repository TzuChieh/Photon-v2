#pragma once

#include "ph_core.h"
#include "JObject.h"

namespace ph
{

class JIntRef final : public JObject<JIntRef>
{
public:
	JIntRef(const jobject javaObject, JNIEnv* const env);

	PHint32 getValue() const;
	void setValue(const PHint32 value);
};

}// end namespace ph