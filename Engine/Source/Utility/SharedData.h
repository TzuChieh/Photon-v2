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
	SharedData(DataArgTypes&&... dataArgs) : 
		m_refCountedData(new RefCountedData(std::forward<DataArgTypes>(dataArgs)...))
	{

	}

	SharedData() : 
		m_refCountedData(nullptr)
	{

	}

private:
	RefCountedData* m_refCountedData;
};// end class SharedData<T>

}// end namespace ph