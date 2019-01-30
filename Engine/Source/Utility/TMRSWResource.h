#pragma once

#include "Utility/INoncopyable.h"

#include <type_traits>
#include <utility>
#include <mutex>
#include <shared_mutex>

namespace ph
{

/*
	A resource wrapper that facilitates Multiple-Readers Single-Writer (MRSW)
	usages. Concurrent access is allowed for read-only operations, while 
	write operations are performed only when exclusive access is aquired.
*/
template<typename Resource>
class TMRSWResource : public INoncopyable
{
public:
	template<typename = std::enable_if_t<std::is_copy_constructible_v<Resource>>>
	TMRSWResource(const Resource& resource);

	template<typename = std::enable_if_t<std::is_move_constructible_v<Resource>>>
	TMRSWResource(Resource&& resource);

	// Performs reading operations on the resource. It is guaranteed no 
	// writing operations are ongoing in the meantime.
	template<typename Reader>
	void read(Reader reader) const;

	// Performs writing operations on the resource. It is guaranteed no 
	// reading operations are ongoing in the meantime.
	template<typename Writer>
	void write(Writer writer);

	// Similar to read(1) except that if the attempt failed, read operation
	// is not performed and false is returned.
	template<typename Reader>
	bool tryRead(Reader reader) const;

	// Similar to write(1) except that if the attempt failed, write operation
	// is not performed and false is returned.
	template<typename Writer>
	bool tryWrite(Writer writer);

	// Performs direct operations on the resource. The operations are 
	// performed straightforwardly without any thread-safety measure.
	template<typename Operation>
	void directCall(Operation operation);

private:
	mutable std::shared_mutex m_mutex;
	Resource                  m_resource;
};

// In-header Implementations:

template<typename Resource>
template<typename>
inline TMRSWResource<Resource>::TMRSWResource(const Resource& resource) : 
	m_mutex(), m_resource(resource)
{}

template<typename Resource>
template<typename>
inline TMRSWResource<Resource>::TMRSWResource(Resource&& resource) : 
	m_mutex(), m_resource(std::move(resource))
{}

template<typename Resource>
template<typename Reader>
inline void TMRSWResource<Resource>::read(Reader reader) const
{
	std::shared_lock lock(m_mutex);

	reader(m_resource);
}

template<typename Resource>
template<typename Writer>
inline void TMRSWResource<Resource>::write(Writer writer)
{
	std::unique_lock lock(m_mutex);

	writer(m_resource);
}

template<typename Resource>
template<typename Reader>
inline bool TMRSWResource<Resource>::tryRead(Reader reader) const
{
	std::shared_lock lock(m_mutex, std::try_to_lock);

	if(lock.owns_lock())
	{
		reader(m_resource);
		return true;
	}
	else
	{
		return false;
	}
}

template<typename Resource>
template<typename Writer>
inline bool TMRSWResource<Resource>::tryWrite(Writer writer)
{
	std::unique_lock lock(m_mutex, std::try_to_lock);

	if(lock.owns_lock())
	{
		writer(m_resource);
		return true;
	}
	else
	{
		return false;
	}
}

template<typename Resource>
template<typename Operation>
inline void TMRSWResource<Resource>::directCall(Operation operation)
{
	operation(m_resource);
}

}// end namespace ph