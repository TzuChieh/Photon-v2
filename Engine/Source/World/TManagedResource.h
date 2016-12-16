#pragma once

#include "Common/primitive_type.h"

namespace ph
{

class World;

//typedef uint32 ResourceId;

template<typename T>
class TManagedResource
{
	friend class World;

public:
	virtual ~TManagedResource() = 0;

private:
	/*ResourceId m_id;

	static const ResourceId INVALID_RESOUR*/

	/*virtual bool loadState() = 0;
	virtual bool saveState() = 0;*/
};

// implementations:

template<typename T>
TManagedResource<T>::~TManagedResource() = default;

}// end namespace ph