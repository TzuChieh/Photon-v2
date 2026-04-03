#pragma once

#include "Common/Utility/TFunction.h"
#include "Engine/SDL/ESdlDataFormat.h"
#include "Engine/SDL/ESdlDataType.h"
#include "Engine/Math/Color/spectrum_fwd.h"
#include "Engine/SDL/TSdlAnyInstance.h"
#include "Engine/Utility/TAnyPtr.h"

#include <Common/primitive_type.h>

#include <cstddef>
#include <variant>
#include <string>
#include <typeindex>
#include <optional>
#include <type_traits>

namespace ph
{

template<typename Variant>
class TSdlAccessorVariant final
{
public:
	using VariantType = Variant;

	TSdlAccessorVariant();

	template<typename T>
	TSdlAccessorVariant(T value);

	TSdlAccessorVariant(VariantType variant);

	template<typename T>
	bool has() const;

	template<typename T>
	T& get();

	template<typename T>
	const T& get() const;

	template<typename T>
	void set(T value);

	std::size_t index() const;
	bool isEmpty() const;

	auto getVariant() -> VariantType&;
	auto getVariant() const -> const VariantType&;

private:
	VariantType m_variant;
};

using SdlGetterVariant = TSdlAccessorVariant<std::variant<
	std::monostate,
	int64,
	float64,
	SdlConstInstance,
	AnyConstPtr>>;

using SdlSetterVariant = TSdlAccessorVariant<std::variant<
	std::monostate,
	int64,
	float64,
	SdlNonConstInstance,
	AnyNonConstPtr>>;

/*!
All public fields are only hints and may not always be available. They provide additional information
for the underlying data, which can help to better interpret them. Implementation ensures the
`SdlNativeData` instance itself has no dynamic allocation and the size of the object should be
reasonably small (independent of the data it is representing). This object can be cached if only
getter and setter accessors are used.

Note on the implementation of getter & setter accessors:
Implementation should guarantee that any calls to getter & setter accessors will not cause the
accessors to be invalid (invalidated) in any way, so an instance of `SdlNativeData` can be reused
many times. Use of direct accessor may invalidate getter and setter accessors. See corresponding
methods for more information.

Note on getting hints for interpreting native data:
If the operation done on the native data would potentially alter the underlying data structure, it is
advisable to retrieve a new instance of `SdlNativeData` to see the updated hint.
*/
class SdlNativeData final
{
public:
	template<typename Func>
	using TElementAccessor = TFunction<Func, 32>;

	using ElementGetter = TElementAccessor<SdlGetterVariant(std::size_t elementIdx)>;
	using ElementSetter = TElementAccessor<bool(std::size_t elementIdx, SdlSetterVariant input)>;

	/*! @brief Hint for number of elements in this block of native data.
	For example, `numElements` would be 12 for an array of 12 `int`s; and 24 for an array of 12 `vec2`s.
	*/
	std::size_t numElements = 0;

	/*! @brief Hint for number of elements that form a natural group.
	For an array of 10 `vec3`s, `tupleSize` may have a value of 3 to indicate that for those 30 elements
	in the array every 3 elements can form a group. This information is not always provided.
	*/
	std::size_t tupleSize = 0;

	/*! @brief Hint for the type that encapsulates elements.
	*/
	ESdlDataFormat elementContainer = ESdlDataFormat::None;

	/*! @brief Hint for the type of elements.
	*/
	ESdlDataType elementType = ESdlDataType::None;

	/*! @brief Whether the data can be set as empty by assigning null to it.
	For example, if this flag is `true` and the data is with a `ESdlDataFormat::Single` format, you
	can use `set(0, nullptr)` to clear it (and use `set(0, T{})` to set it). Other formats and types
	follow the same principle.
	*/
	uint8 isNullClearable : 1 = false;

	/*! @brief Creates native data for a single element pointer.
	@param elementPtr Pointer to the single element. If null, all native data access will be no-op.
	@param elementContainer Format of the data container.
	@param elementType Type of the data element.
	@param canSet If true, @p elementPtr will also be used for ordinary setter.
	@param canDirectAccess If true, @p elementPtr will also be used for direct access.
	*/
	template<typename ElementType>
	static SdlNativeData fromSingleElement(
		ElementType* elementPtr,
		ESdlDataFormat elementContainer,
		ESdlDataType elementType,
		bool canSet = false, 
		bool canDirectAccess = false);

	/*! @brief Creates empty native data.
	*/
	SdlNativeData();

	/*! @brief Creates read-only native data.
	@param getter Functor for retrieving element values.
	*/
	explicit SdlNativeData(ElementGetter getter);

	/*! @brief Creates native data with both read and write capabilities.
	@param getter Functor for retrieving element values.
	@param setter Functor for setting element values.
	*/
	SdlNativeData(ElementGetter getter, ElementSetter setter);

	/*! @brief Creates native data with custom capabilities.
	@param getter Functor for retrieving element values.
	@param setter Functor for setting element values.
	@param directPtr Pointer for direct memory access.
	*/
	SdlNativeData(ElementGetter getter, ElementSetter setter, AnyNonConstPtr directPtr);

	/*! @brief Get the value of an element.
	@param elementIdx Index of the element to retrieve.
	@return The value of the element, or `std::nullopt` if retrieval failed or index out of bounds.
	*/
	template<typename T>
		requires std::is_arithmetic_v<T> || std::is_enum_v<T>
	std::optional<T> get(std::size_t elementIdx) const;

	/*! @brief Get value of an element.
	@param elementIdx Index of the element to retrieve.
	@return The pointer to value, or `nullptr` if retrieval failed or index out of bounds.
	*/
	template<typename T>
		requires std::is_pointer_v<T>
	T get(std::size_t elementIdx) const;

	/*! @brief Set the value of an element.
	@param elementIdx Index of the element to set.
	@param value The new value.
	@return `true` if the value was successfully set, `false` otherwise.
	*/
	template<typename T>
		requires std::is_arithmetic_v<T> || std::is_enum_v<T>
	bool set(std::size_t elementIdx, T value) const;

	/*! @brief Set the value of an element.
	@param elementIdx Index of the element to set.
	@param ptr Pointer to the new value.
	@return `true` if the value was successfully set, `false` otherwise.
	*/
	template<typename T>
		requires std::is_pointer_v<T>
	bool set(std::size_t elementIdx, T ptr) const;

	/*! @brief Set an element to null/empty state.
	See `isNullClearable`.
	@param elementIdx Index of the element to clear.
	@param nullPtr Null pointer constant (`nullptr`).
	@return `true` if the element was successfully cleared, `false` otherwise.
	*/
	template<typename T>
		requires std::is_null_pointer_v<T>
	bool set(std::size_t elementIdx, T nullPtr) const;

	/*! @brief Directly access the underlying data.
	Use of direct accessor may invalidate getter and setter accessors (should reacquire native data
	from source for subsequent getter/setter access). Direct accessor never invalidate itself.
	@return Pointer to the underlying data.
	*/
	template<typename T>
	T* directAccess() const;

	/*! @brief Set the direct accessor pointer.
	@param accessor Pointer to the underlying data.
	*/
	void setDirectAccessor(AnyNonConstPtr accessor);

	/*! @brief Check if elements are of integer type.
	*/
	bool isIntegerElement() const;

	/*! @brief Check if elements are of floating-point type.
	*/
	bool isFloatingPointElement() const;

	/*! @brief Check if the data is valid.
	*/
	operator bool () const;

public:
	/*! @brief Given a valid target element, get its value in a permissive way (with auto conversions).
	@param elementPtr Pointer to the target element. Cannot be null.
	*/
	template<typename ElementType>
	static auto permissiveElementGetter(ElementType* elementPtr) -> SdlGetterVariant;

	/*! @brief Given a valid target element, set its value in a permissive way (with auto conversions).
	@param out_elementPtr Pointer to the target element. Cannot be null.
	*/
	template<typename ElementType>
	static auto permissiveElementSetter(SdlSetterVariant input, ElementType* out_elementPtr) -> bool;

private:
	ElementGetter m_elementGetter;
	ElementSetter m_elementSetter;
	AnyNonConstPtr m_directPtr;
};

}// end namespace ph

#include "Engine/SDL/Introspect/SdlNativeData.ipp"
