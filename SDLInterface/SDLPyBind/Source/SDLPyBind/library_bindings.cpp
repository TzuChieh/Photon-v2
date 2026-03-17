#include "library_bindings.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>
#include <Engine/ph_core.h>
#include <Engine/SDL/Introspect/SdlInstantiated.h>
#include <Engine/SDL/Introspect/SdlNativeData.h>

#include <nanobind/ndarray.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/string_view.h>

#include <unordered_map>

namespace ph::py
{

void bind_engine_sdl_definitions(nanobind::module_ _)
{
	nanobind::module_ m = _.def_submodule("engine", "Core render engine.");

	UniversalSDLBinder binder(m);
	bind_engine(binder);
}

std::string UniversalSDLBinder::toSdlTypeName(nanobind::handle pyType)
{
	// See the nanobind doc for exchanging information
	// https://nanobind.readthedocs.io/en/latest/exchanging.html

	static const std::unordered_map<const PyObject*, std::string> pyTypeToSdlTypeName = 
		[]()
		{
			std::unordered_map<const PyObject*, std::string> map;

			map[nanobind::type<nanobind::bool_>().ptr()] = "bool";
			map[nanobind::type<nanobind::int_>().ptr()] = "integer";
			map[nanobind::type<nanobind::float_>().ptr()] = "real";
			map[nanobind::type<nanobind::str>().ptr()] = "string";

			auto pathlib = nanobind::module_::import_("pathlib");
			map[pathlib.attr("Path").ptr()] = "path";
			map[pathlib.attr("PurePosixPath").ptr()] = "path";
			map[pathlib.attr("PureWindowsPath").ptr()] = "path";
			if(nanobind::hasattr(pathlib, "PosixPath"))
			{
				map[pathlib.attr("PosixPath").ptr()] = "path";
			}
			if(nanobind::hasattr(pathlib, "WindowsPath"))
			{
				map[pathlib.attr("WindowsPath").ptr()] = "path";
			}

			return map;
		}();

	auto mapResult = pyTypeToSdlTypeName.find(pyType.ptr());
	if(mapResult != pyTypeToSdlTypeName.end())
	{
		return mapResult->second;
	}
	else
	{
		throw_nanobind_type_error(
			"unable to map Python value type <{}> to SDL",
			nanobind::cast<std::string>(nanobind::str(pyType)));
	}
}

SdlInputClauses UniversalSDLBinder::toSdlInputClauses(nanobind::kwargs kwargs)
{
	SdlInputClauses clauses;
	for(auto [key, value] : kwargs)
	{
		clauses.add(
			SdlInputClause(
				toSdlTypeName(value.type()),
				nanobind::cast<std::string>(nanobind::str(key)),
				nanobind::cast<std::string>(nanobind::str(value))));
	}

	return clauses;
}

namespace
{

template<typename PrimitiveType>
inline bool copy_ndarray_to_primitive_vector(nanobind::handle pyValue, SdlNativeData& nativeData)
{
	using NativeType = std::vector<PrimitiveType>;

	if(!nanobind::ndarray_check(pyValue))
	{
		return false;
	}

	auto* cppData = nativeData.directAccess<NativeType>();
	if(!cppData)
	{
		return false;
	}

	// Now the conditions for direct copying to happen are met.

	auto pyNdarray = nanobind::cast<nanobind::ndarray<PrimitiveType, nanobind::c_contig, nanobind::device::cpu>>(pyValue);
	if(pyNdarray.ndim() != 1)
	{
		throw_nanobind_value_error(
			"expected array dimension 1, {} was given", pyNdarray.ndim());
	}

	PrimitiveType* pyData = pyNdarray.data();
	if(pyData)
	{
		cppData->assign(pyData, pyData + pyNdarray.shape(0));
	}
	else
	{
		cppData->clear();
	}

	return true;
}

}// end namespace

bool UniversalSDLBinder::tryTransferToSdlNativeData(nanobind::handle pyValue, SdlNativeData& nativeData)
{
	if(!nativeData)
	{
		return false;
	}

	switch(nativeData.elementContainer)
	{
	case ESdlDataFormat::Vector:
	{
		switch(nativeData.elementType)
		{
		// FIXME: this likely will fail due to std::vector<bool> specialization
		case ESdlDataType::Bool:
			return copy_ndarray_to_primitive_vector<bool>(pyValue, nativeData);

		case ESdlDataType::Int8:
			return copy_ndarray_to_primitive_vector<int8>(pyValue, nativeData);

		case ESdlDataType::Int16:
			return copy_ndarray_to_primitive_vector<int16>(pyValue, nativeData);

		case ESdlDataType::Int32:
			return copy_ndarray_to_primitive_vector<int32>(pyValue, nativeData);

		case ESdlDataType::Int64:
			return copy_ndarray_to_primitive_vector<int64>(pyValue, nativeData);

		case ESdlDataType::UInt8:
			return copy_ndarray_to_primitive_vector<uint8>(pyValue, nativeData);

		case ESdlDataType::UInt16:
			return copy_ndarray_to_primitive_vector<uint16>(pyValue, nativeData);

		case ESdlDataType::UInt32:
			return copy_ndarray_to_primitive_vector<uint32>(pyValue, nativeData);

		case ESdlDataType::UInt64:
			return copy_ndarray_to_primitive_vector<uint64>(pyValue, nativeData);

		case ESdlDataType::Float32:
			return copy_ndarray_to_primitive_vector<float32>(pyValue, nativeData);

		case ESdlDataType::Float64:
			return copy_ndarray_to_primitive_vector<float64>(pyValue, nativeData);
		}// end switch ESdlDataType
	}
	}// end switch ESdlDataFormat

	return false;
}

void UniversalSDLBinder::callSdlStaticFunction(
	const SdlFunction& sdlFunc,
	const std::vector<const SdlField*>& nativeAccessParams,
	nanobind::kwargs kwargs)
{
	if(nativeAccessParams.empty())
	{
		SdlInputClauses clauses = toSdlInputClauses(kwargs);
		sdlFunc.call(
			nullptr,
			clauses,
			SdlInputContext{});
	}
	else
	{
		SdlInstantiated params = sdlFunc.instantiate();
		if(params)
		{
			for(const SdlField* nativeAccessParam : nativeAccessParams)
			{
				nanobind::object pyKey = nanobind::cast(nativeAccessParam->getSnakeCaseFieldName());
				nanobind::object pyValue = kwargs.attr("pop")(pyKey, nanobind::none());
				if(pyValue.is_none())
				{
					continue;
				}

				SdlNativeData nativeData = nativeAccessParam->nativeData(params.data);
				if(!tryTransferToSdlNativeData(pyValue, nativeData))
				{
					// Transfer failed, put it back for generating input clauses
					kwargs[pyKey] = pyValue;
				}
			}
		}

		// We still need to build clauses for params that do not prefer native access
		// or native access somehow failed
		SdlInputClauses clauses = toSdlInputClauses(kwargs);

		sdlFunc.call(
			nullptr,
			params.data,
			clauses,
			SdlInputContext{});
	}
}

std::string UniversalSDLBinder::toRestructuredTextDocstring(const ISdlInstantiable& instantiableType)
{
	std::string docstring{instantiableType.getDescription()};
	docstring += "\n\n";
	for(std::size_t pi = 0; pi < instantiableType.numFields(); ++pi)
	{
		const SdlField* sdlField = instantiableType.getField(pi);
		docstring += ":param " + std::string(sdlField->getSnakeCaseFieldName()) + ": ";
		docstring += std::string(sdlField->getDescription()) + "\n";
		docstring += ":type " + std::string(sdlField->getSnakeCaseFieldName()) + ": ";
		docstring += std::string(sdlField->getTypeName()) + "\n";
	}

	return docstring;
}

}// end namespace ph::py
