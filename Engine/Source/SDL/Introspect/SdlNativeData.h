#pragma once

#include "Common/primitive_type.h"
#include "SDL/ESdlDataFormat.h"
#include "SDL/ESdlDataType.h"
#include "Utility/TFunction.h"
#include "Math/Color/spectrum_fwd.h"
#include "SDL/TSdlAnyInstance.h"
#include "Utility/TAnyPtr.h"

#include <cstddef>
#include <variant>
#include <string>
#include <typeindex>
#include <optional>
#include <type_traits>

namespace ph
{

/*!
Implementation ensures there is no dynamic allocation and the size of the object should be reasonably
small (independent of the data it is representing). This object can be cached if only getter and setter
accessors are used. Use of direct accessor may invalidate getter and setter accessors. See corresponding
methods for more information.
*/
class SdlNativeData final
{
public:
	using GetterVariant = std::variant<
		std::monostate,
		int64,
		float64,
		SdlConstInstance,
		AnyConstPtr>;

	using SetterVariant = std::variant<
		std::monostate,
		int64,
		float64,
		SdlNonConstInstance,
		AnyNonConstPtr>;

	template<typename Func>
	using TElementAccessor = TFunction<Func, 32>;

	using ElementGetter = TElementAccessor<GetterVariant(std::size_t elementIdx)>;
	using ElementSetter = TElementAccessor<bool(std::size_t elementIdx, SetterVariant input)>;

	ESdlDataFormat elementContainer = ESdlDataFormat::None;
	ESdlDataType elementType = ESdlDataType::None;

	/*! @brief Number of elements in this block of native data.
	For example, `numElements` would be 12 for an array of 12 `int`s; and 24 for an array of 12 `vec2`s.
	*/
	std::size_t numElements = 0;

	/*! @brief Hint for number of elements that form a natural group.
	For an array of 10 `vec3`s, `tupleSize` may have a value of 3 to indicate that for those 30 elements
	in the array every 3 elements can form a group. This information is not always provided.
	*/
	std::size_t tupleSize = 0;

	/*! @brief Creates native data for a single element pointer.
	@param canSet If true, @p elementPtr will also be used for ordinary setter.
	@param canDirectAccess If true, @p elementPtr will also be used for direct access.
	*/
	template<typename ElementType>
	static SdlNativeData fromSingleElement(
		ElementType* elementPtr, 
		bool canSet = false, 
		bool canDirectAccess = false);

	/*! @brief Creates empty native data.
	*/
	SdlNativeData();

	/*! @brief Creates read-only native data.
	*/
	explicit SdlNativeData(ElementGetter getter);

	/*! @brief Creates native data with both read and write capabilities.
	*/
	SdlNativeData(ElementGetter getter, ElementSetter setter);

	/*! @brief Creates native data with custom capabilities.
	*/
	SdlNativeData(ElementGetter getter, ElementSetter setter, AnyNonConstPtr directPtr);

	template<typename T> requires std::is_arithmetic_v<T> || std::is_enum_v<T>
	std::optional<T> get(std::size_t elementIdx) const;

	template<typename T> requires std::is_pointer_v<T>
	T get(std::size_t elementIdx) const;

	template<typename T> requires std::is_arithmetic_v<T> || std::is_enum_v<T>
	bool set(std::size_t elementIdx, T value) const;

	template<typename T> requires std::is_pointer_v<T>
	bool set(std::size_t elementIdx, T ptr) const;

	/*! @brief Directly access the underlying data.
	Use of direct accessor may invalidate getter and setter accessors (should reacquire native data
	from source for subsequent getter/setter access). Direct accessor never invalidate itself.
	*/
	template<typename T>
	T* directAccess() const;

	void setDirectAccessor(AnyNonConstPtr accessor);

	bool isIntegerElement() const;
	bool isFloatingPointElement() const;

	operator bool () const;

public:
	template<typename ElementType>
	static auto permissiveElementToGetterVariant(ElementType* elementPtr) -> GetterVariant;

	template<typename ElementType>
	static auto permissiveSetterVariantToElement(SetterVariant input, ElementType* out_elementPtr) -> bool;

private:
	ElementGetter m_elementGetter;
	ElementSetter m_elementSetter;
	AnyNonConstPtr m_directPtr;
};

}// end namespace ph

#include "SDL/Introspect/SdlNativeData.ipp"
