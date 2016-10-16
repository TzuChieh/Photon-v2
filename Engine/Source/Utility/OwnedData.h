#pragma once

#include <utility>

namespace ph
{

template<typename DataType>
class OwnedData final
{
public:
	template<typename... DataArgTypes>
	OwnedData(DataArgTypes&&... dataArgs) : 
		m_data(new DataType(std::forward<DataArgTypes>(dataArgs)...))
	{

	}

	// forbid copying
	template<typename T>
	OwnedData(const OwnedData<T>& other) = delete;
	template<typename T>
	OwnedData& operator = (const OwnedData<T>& rhs) = delete;

	~OwnedData()
	{
		delete m_data;
	}

	DataType* operator -> () const
	{
		return m_data;
	}

private:
	DataType* m_data;
};

}// end namespace ph