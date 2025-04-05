#pragma once

namespace ph
{

/*! @brief Marks the derived class as uninstantiable.

Deriving from this class helps to prevent accidentally creating an instance
of the class. This class should never be used as a base type that points to 
derived class instances.
*/
class IUninstantiable
{
protected:
	IUninstantiable() = delete;
	IUninstantiable(const IUninstantiable& other) = delete;
	IUninstantiable& operator = (const IUninstantiable& rhs) = delete;
	IUninstantiable(IUninstantiable&& other) = delete;
	IUninstantiable& operator = (IUninstantiable&& rhs) = delete;

	// Not deleting dtor--deleting dtor will not prevent instantiation via `new`,
	// defined as default just in case some corner cases may need it to cleanup.
	inline ~IUninstantiable() = default;
};

}// end namespace ph
