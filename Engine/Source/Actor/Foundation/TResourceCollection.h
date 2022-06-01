#pragma once

#include <vector>
#include <memory>

namespace ph
{

template<typename BaseResourceType>
class TResourceCollection final
{
public:
	// TODO: emplace



private:
	std::vector<std::unique_ptr<BaseResourceType>> m_resources;
};

}// end namespace ph
