#pragma once

#include "Utility/INoCopyAndMove.h"
#include "Common/assertion.h"

#include <shared_mutex>
#include <type_traits>
#include <utility>
#include <memory>

namespace ph
{

/*! @brief Synchronized value wrapper for shared read and exclusive write accesses.
As many of the other concurrent utilities provided by Photon, ctor and dtor accesses are not 
thread-safe, and it is the programmer's responsibility to ensure all accesses to this class and its
derived temporaries are within the class's lifetime.

An important note that this class relies on the face that const methods from the wrapped type are safe
to access (read) concurrently, and non-const methods are unsafe to access (write) without exclusive
locking. Later standards of C++ (C++11) enforces this and thus most STL classes are safe to use with 
this type. The interface deliberately avoid the wordings of read & write locking as one can change
the mutex type @p Mutex to achieve different locking semantics.

Beware that most STL mutexes are not reentrant/recursive, including `std::shared_mutex` (while 
Andrei's talk [1] said otherwise). Also, two interacting `TSynchronized` may induce deadlock if both
thread acquired a write lock from each of them then acquired a read lock from another one. E.g., 

```C++

    thread 1:              thread 2: 
s1.nonConstLock();     s2.nonConstLock();
s2.constLock();        s1.constLock();

```

An easy solution to this problem is to break the cycle--always (write) lock the lower address first 
(see [1] for more details). Currently this operation is not supplied by the class since our class 
interface simplicitly do not support such operation explicitly (e.g., assigning a `TSynchronized` to
another one). This is just a reminder of a potentially hard-to-debug misuse.

References:
[1] NDC Oslo talk "Generic Locking in C++" by Andrei Alexandrescu
[2] Facebook's folly library (folly::Synchronized)
*/
template<typename T, typename Mutex = std::shared_mutex>
class TSynchronized final : private INoCopyAndMove
{
private:
	/*! @brief Lock the wrapped value for the duration of this instance's lifetime.
	Temporary created from `TSynchronized::operator ->` for writers. Act similarly as `std::lock_guard`.
	*/
	class AutoLockingPtr final : private INoCopyAndMove
	{
	public:
		explicit AutoLockingPtr(TSynchronized& parent)
			: m_parent(parent)
		{
			m_parent.m_mutex.lock();
		}

		~AutoLockingPtr()
		{
			m_parent.m_mutex.unlock();
		}

		T* operator -> ()
		{
			return std::addressof(m_parent.m_value);
		}

		T& operator * ()
		{
			return m_parent.m_value;
		}

	private:
		TSynchronized& m_parent;
	};

	/*! @brief Lock the wrapped value for the duration of this instance's lifetime.
	Temporary created from `TSynchronized::operator ->` for readers. Act similarly as `std::lock_guard`.
	*/
	class AutoConstLockingPtr final : private INoCopyAndMove
	{
	public:
		explicit AutoConstLockingPtr(const TSynchronized& parent)
			: m_parent(parent)
		{
			m_parent.m_mutex.lock_shared();
		}

		~AutoConstLockingPtr()
		{
			m_parent.m_mutex.unlock_shared();
		}

		const T* operator -> () const
		{
			return std::addressof(m_parent.m_value);
		}

		const T& operator * () const
		{
			return m_parent.m_value;
		}

	private:
		const TSynchronized& m_parent;
	};

public:
	TSynchronized() = default;

	explicit TSynchronized(const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>)
		: m_value(value)
		, m_mutex()
	{}

	explicit TSynchronized(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>)
		: m_value(std::move(value))
		, m_mutex()
	{}

	/*! @brief Convenient const qualifier for potentially cheaper read locking.
	*/
	const TSynchronized& asConst() const
	{
		return *this;
	}

	AutoLockingPtr lockedPtr()
	{
		// Relied on mandatory copy elision
		return AutoLockingPtr(*this);
	}

	AutoConstLockingPtr constLockedPtr() const
	{
		// Relied on mandatory copy elision
		return AutoConstLockingPtr(*this);
	}

	template<typename LockedFunc>
	void locked(LockedFunc func)
	{
		static_assert(std::is_invocable_v<LockedFunc, T&>,
			"LockedFunc must take (T&).");

		AutoLockingPtr lockedPtr(*this);
		func(*lockedPtr);
	}

	template<typename ConstLockedFunc>
	void constLocked(ConstLockedFunc func) const
	{
		static_assert(std::is_invocable_v<ConstLockedFunc, const T&>,
			"ConstLockedFunc must take (const T&).");

		AutoConstLockingPtr lockedPtr(*this);
		func(*lockedPtr);
	}

	T makeCopy() const
	{
		AutoConstLockingPtr lockedPtr(*this);
		return *lockedPtr;
	}

	TSynchronized& operator = (const T& rhsValue)
	{
		PH_ASSERT(&m_value != &rhsValue);

		AutoLockingPtr lockedPtr(*this);
		*lockedPtr = rhsValue;

		return *this;
	}

	TSynchronized& operator = (T&& rhsValue)
	{
		PH_ASSERT(&m_value != &rhsValue);

		AutoLockingPtr lockedPtr(*this);
		*lockedPtr = std::move(rhsValue);

		return *this;
	}

	/*! @brief Access to a member of wrapped value with automatic write locking/unlocking.
	*/
	AutoLockingPtr operator -> ()
	{
		// Relied on mandatory copy elision
		return AutoLockingPtr(*this);
	}

	/*! @brief Access to a member of wrapped value with automatic read locking/unlocking.
	*/
	AutoConstLockingPtr operator -> () const
	{
		// Relied on mandatory copy elision
		return AutoConstLockingPtr(*this);
	}

private:
	T m_value;
	mutable Mutex m_mutex;
};

}// end namespace ph
