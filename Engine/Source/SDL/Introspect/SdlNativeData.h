#pragma once

#include "Common/primitive_type.h"
#include "SDL/ESdlDataFormat.h"
#include "SDL/ESdlDataType.h"
#include "Utility/TFunction.h"
#include "Math/Color/spectrum_fwd.h"

#include <cstddef>
#include <variant>
#include <string>
#include <typeindex>

namespace ph
{

class Path;
class ResourceIdentifier;
class Geometry;
class Material;
class Motion;
class LightSource;
class Actor;
class Image;
class FrameProcessor;
class Observer;
class SampleSource;
class Visualizer;
class Option;
class Object;

class SdlNativeData final
{
public:
	using GetterVariant = std::variant<
		std::monostate,
		int64,
		uint64,
		float64,
		const std::string*,
		const Path*,
		const ResourceIdentifier*,
		const math::Spectrum*,
		const Geometry*,
		const Material*,
		const Motion*,
		const LightSource*,
		const Actor*,
		const Image*,
		const FrameProcessor*,
		const Observer*,
		const SampleSource*,
		const Visualizer*,
		const Option*,
		const Object*>;

	using SetterVariant = std::variant<
		std::monostate,
		int64,
		uint64,
		float64,
		std::string*,
		Path*,
		ResourceIdentifier*,
		math::Spectrum*,
		Geometry*,
		Material*,
		Motion*,
		LightSource*,
		Actor*,
		Image*,
		FrameProcessor*,
		Observer*,
		SampleSource*,
		Visualizer*,
		Option*,
		Object*>;

	ESdlDataFormat format = ESdlDataFormat::None;
	ESdlDataType dataType = ESdlDataType::None;
	std::size_t numElements = 0;
	uint8 tupleSize = 0;

	SdlNativeData();

	template<typename ElementType>
	explicit SdlNativeData(ElementType* elementPtr);

	template<typename ElementGetterFunc>
	SdlNativeData(ElementGetterFunc func, std::size_t numElements);

	/*template<typename T>
	T* directAccess() const;*/

	void* operator [] (std::size_t elementIdx) const;

	operator bool() const;

private:
	TFunction<void* (std::size_t elementIdx)> m_elementGetter;
	void* m_directPtr;
	//std::type_index m_directType;
};

}// end namespace ph

#include "SDL/Introspect/SdlNativeData.ipp"
