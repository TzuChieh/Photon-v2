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
	std::size_t numElements = 0;
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

	template<typename T>
	T* directAccess() const;

	void setDirectAccessor(AnyNonConstPtr accessor);

	operator bool() const;

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
