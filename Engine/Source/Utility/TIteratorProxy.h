#pragma once

namespace ph
{

template<typename ContainerType>
class TIteratorProxy
{
public:
	explicit TIteratorProxy(ContainerType& container) :
		m_container(container)
	{

	}

	inline typename ContainerType::iterator begin()
	{
		return m_container.begin();
	}

	inline typename ContainerType::iterator end()
	{
		return m_container.end();
	}

private:
	ContainerType& m_container;
};

}// end namespace ph