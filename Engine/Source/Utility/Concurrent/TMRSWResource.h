#pragma once

#include "Utility/INoCopyAndMove.h"

#include <type_traits>
#include <utility>
#include <mutex>
#include <shared_mutex>
#include <iostream>

namespace ph
{

/*! @brief A resource wrapper that facilitates Multiple-Readers Single-Writer (MRSW) usages.

Concurrent access is allowed for read-only operations, while write operations
are performed only when exclusive access is acquired.
*/
// TODO: convenient getter and setter
// TODO: perfect forward readers, writers, and operations?
template<typename Resource>
class TMRSWResource final : private INoCopyAndMove
{
public:
	template<typename Res>
	explicit TMRSWResource(Res&& resource);

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

	// Note that the type contains mutex and should be non-copyable and
	// non-movable already, but the template constructor for stored resources
	// seems to fool some type_traits functionalities (the INoCopyAndMove base
	// cannot help with the case of std::is_move_constructible_v, it returns
	// true always in VS 15.9.34);
	// made the copy and move requirements explicit by marking them as
	// deleted here, which will solve the issue (as these are better match
	// than templates).
	TMRSWResource(const TMRSWResource& other) = delete;
	TMRSWResource& operator = (const TMRSWResource& rhs) = delete;
	TMRSWResource(TMRSWResource&& other) = delete;
	TMRSWResource& operator = (TMRSWResource&& rhs) = delete;

private:
	mutable std::shared_mutex m_mutex;
	Resource                  m_resource;
};

// In-header Implementations:

template<typename Resource>
template<typename Res>
inline TMRSWResource<Resource>::TMRSWResource(Res&& resource) :
	m_mutex(), m_resource(std::forward<Res>(resource))
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
