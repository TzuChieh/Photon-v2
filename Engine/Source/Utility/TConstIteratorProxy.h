#pragma once

namespace ph
{

template<typename ContainerType>
class TConstIteratorProxy
{
public:
	TConstIteratorProxy(const ContainerType& container) : 
		m_container(container)
	{

	}

	inline typename ContainerType::const_iterator begin() const
	{
		return m_container.begin();
	}

	inline typename ContainerType::const_iterator end() const
	{
		return m_container.end();
	}

private:
	const ContainerType& m_container;
};

}// end namespace ph