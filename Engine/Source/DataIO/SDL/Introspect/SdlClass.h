#pragma once

#include <vector>
#include <memory>

namespace ph
{

class SdlField;

class SdlClass
{
public:
	virtual ~SdlClass() = default;

private:
	std::vector<std::shared_ptr<SdlField>> m_fields;
};

}// end namespace ph
