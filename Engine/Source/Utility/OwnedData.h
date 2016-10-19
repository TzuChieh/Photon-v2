#pragma once

#include <utility>
#include <cstddef>

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

	OwnedData(nullptr_t empty) :
		m_data(empty)
	{

	}

	OwnedData() :
		OwnedData(nullptr)
	{

	}

	OwnedData(OwnedData&& other) :
		m_data(other.m_data)
	{
		other.m_data = nullptr;
	}

	~OwnedData()
	{
		delete m_data;
	}

	// forbid copying
	template<typename T>
	OwnedData(const OwnedData<T>& other) = delete;
	template<typename T>
	OwnedData& operator = (const OwnedData<T>& rhs) = delete;

	DataType* lend() const
	{
		return m_data;
	}

	DataType* operator -> () const
	{
		return m_data;
	}

	DataType& operator * () const
	{
		return *m_data;
	}

	OwnedData& operator = (OwnedData&& other)
	{
		if(this != &other)
		{
			delete m_data;
			m_data = other.m_data;
			other.m_data = nullptr;
		}

		return *this;
	}

private:
	DataType* m_data;
};

}// end namespace ph