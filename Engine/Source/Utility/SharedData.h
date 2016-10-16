#pragma once

#include "Common/primitive_type.h"

#include <utility>

namespace ph
{

template<typename DataType>
class SharedData final
{
private:
	class RefCountedData final
	{
	public:
		template<typename... DataArgTypes>
		RefCountedData(DataArgTypes&&... dataArgs) : 
			m_data(std::forward<DataArgTypes>(dataArgs)...), m_numRefs(0)
		{
			incrementRefCount();
		}

		void incrementRefCount()
		{
			m_numRefs++;
		}

	public:
		DataType m_data;
		uint64   m_numRefs;
	};

public:
	template<typename... DataArgTypes>
	static SharedData<DataType> create(DataArgTypes&&... dataArgs)
	{
		RefCountedData* refCountedData = new RefCountedData(std::forward<DataArgTypes>(dataArgs)...);
		return SharedData<DataType>(refCountedData);
	}

	SharedData() : 
		m_refCountedData(nullptr)
	{

	}

private:
	RefCountedData* m_refCountedData;

	SharedData(RefCountedData* const refCountedData) : 
		m_refCountedData(refCountedData)
	{

	}
};// end class SharedData<T>

}// end namespace ph