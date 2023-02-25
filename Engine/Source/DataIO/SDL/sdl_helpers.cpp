#include "DataIO/SDL/sdl_helpers.h"
#include "DataIO/SDL/Tokenizer.h"
#include "DataIO/FileSystem/Path.h"
#include "DataIO/io_exceptions.h"
#include "DataIO/SDL/Introspect/SdlClass.h"
#include "DataIO/SDL/Introspect/SdlField.h"
#include "DataIO/SDL/Introspect/SdlFunction.h"
#include "Utility/string_utils.h"
#include "DataIO/SDL/SdlOutputPayload.h"
#include "DataIO/SDL/sdl_exceptions.h"

#include <exception>

namespace ph::sdl
{

math::Vector3R load_vector3(const std::string& sdlVector3Str)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {});

	try
	{
		std::vector<std::string> tokens;
		tokenizer.tokenize(sdlVector3Str, tokens);

		// 3 input values form a vec3 exactly
		if(tokens.size() == 3)
		{
			return math::Vector3R(
				load_real(tokens[0]),
				load_real(tokens[1]),
				load_real(tokens[2]));
		}
		// 1 input value results in vec3 filled with the same value
		else if(tokens.size() == 1)
		{
			return math::Vector3R(load_real(tokens[0]));
		}
		else
		{
			throw SdlLoadError(
				"invalid Vector3R representation "
				"(number of values = " + std::to_string(tokens.size()) + ")");
		}
	}
	catch(const SdlLoadError& e)
	{
		throw SdlLoadError("on parsing Vector3R -> " + e.whatStr());
	}
}

math::QuaternionR load_quaternion(const std::string& sdlQuaternionStr)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {});

	try
	{
		// TODO: handle all equal values

		std::vector<std::string> tokens;
		tokenizer.tokenize(sdlQuaternionStr, tokens);

		if(tokens.size() != 4)
		{
			throw SdlLoadError("invalid QuaternionR representation");
		}

		return math::QuaternionR(
			load_real(tokens[0]),
			load_real(tokens[1]),
			load_real(tokens[2]),
			load_real(tokens[3]));
	}
	catch(const SdlLoadError& e)
	{
		throw SdlLoadError("on parsing QuaternionR -> " + e.whatStr());
	}
}

std::vector<math::Vector3R> load_vector3_array(const std::string& sdlVector3ArrayStr)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {{'\"', '\"'}});

	try
	{
		std::vector<std::string> vec3Tokens;
		tokenizer.tokenize(sdlVector3ArrayStr, vec3Tokens);

		std::vector<math::Vector3R> vec3Array(vec3Tokens.size());
		for(std::size_t i = 0; i < vec3Array.size(); ++i)
		{
			vec3Array[i] = load_vector3(vec3Tokens[i]);
		}

		return vec3Array;
	}
	catch(const SdlLoadError& e)
	{
		throw SdlLoadError("on parsing Vector3R array -> " + e.whatStr());
	}
}

void save_vector3(const math::Vector3R& value, std::string* const out_str)
{
	PH_ASSERT(out_str);

	try
	{
		if(value.x() == value.y() && value.y() == value.z())
		{
			save_real(value.x(), out_str);
		}
		else
		{
			out_str->clear();

			std::string savedReal;

			(*out_str) += '\"';
			save_real(value.x(), &savedReal);
			(*out_str) += savedReal;
			(*out_str) += ' ';
			save_real(value.y(), &savedReal);
			(*out_str) += savedReal;
			(*out_str) += ' ';
			save_real(value.z(), &savedReal);
			(*out_str) += savedReal;
			(*out_str) += '\"';
		}
	}
	catch(const SdlSaveError& e)
	{
		throw SdlSaveError("on saving Vector3R -> " + e.whatStr());
	}
}

void save_quaternion(const math::QuaternionR& value, std::string* const out_str)
{
	PH_ASSERT(out_str);

	try
	{
		// TODO: handle all equal values

		out_str->clear();

		std::string savedReal;

		(*out_str) += '\"';
		save_real(value.x, &savedReal);
		(*out_str) += savedReal;
		(*out_str) += ' ';
		save_real(value.y, &savedReal);
		(*out_str) += savedReal;
		(*out_str) += ' ';
		save_real(value.z, &savedReal);
		(*out_str) += savedReal;
		(*out_str) += ' ';
		save_real(value.w, &savedReal);
		(*out_str) += savedReal;
		(*out_str) += '\"';
	}
	catch(const SdlSaveError& e)
	{
		throw SdlSaveError("on saving QuaternionR -> " + e.whatStr());
	}
}

void save_vector3_array(const std::vector<math::Vector3R>& values, std::string* const out_str)
{
	PH_ASSERT(out_str);

	try
	{
		(*out_str) += '{';

		std::string savedVec3;
		for(const auto& vec3 : values)
		{
			save_vector3(vec3, &savedVec3);
			(*out_str) += savedVec3;
			(*out_str) += ' ';
		}

		(*out_str) += '}';
	}
	catch(const SdlSaveError& e)
	{
		throw SdlSaveError("on saving Vector3R array -> " + e.whatStr());
	}
}

void save_field_id(const SdlField* const sdlField, SdlOutputPayload& payload)
{
	PH_ASSERT(sdlField);

	payload.type = sdlField->getTypeName();
	payload.name = sdlField->getFieldName();
}

bool is_resource_identifier(const std::string_view sdlValueStr)
{
	// Remove leading blank characters
	const auto trimmedStr = string_utils::trim_head(sdlValueStr);

	// Valid SDL resource identifier starts with "/"
	return !trimmedStr.empty() && trimmedStr[0] == '/';
}

bool is_reference(const std::string_view sdlValueStr)
{
	// Remove leading blank characters
	const auto trimmedStr = string_utils::trim_head(sdlValueStr);

	// Valid SDL reference starts with "@"
	return !trimmedStr.empty() && trimmedStr[0] == '@';
}

std::string gen_pretty_name(const SdlClass* const clazz)
{
	return clazz ? 
		"category: " + clazz->genCategoryName() + ", name: " + clazz->getTypeName() : 
		"unavailable";
}

std::string gen_pretty_name(const SdlFunction* const func)
{
	return func ?
		"name: " + func->getName() :
		"unavailable";
}

std::string gen_pretty_name(const SdlField* const field)
{
	return field ?
		"type: " + field->getTypeName() + ", name: " + field->getFieldName() : 
		"unavailable";
}

std::string gen_pretty_name(const SdlClass* const clazz, const SdlField* const field)
{
	return "SDL class <" + gen_pretty_name(clazz) + ">, value <" + gen_pretty_name(field) + ">";
}

auto get_all_callable_functions(const SdlClass* const callableParentClass)
-> std::vector<std::pair<const SdlFunction*, const SdlClass*>>
{
	std::vector<std::pair<const SdlFunction*, const SdlClass*>> results;
	
	const SdlClass* currentClass = callableParentClass;
	while(currentClass)
	{
		for(std::size_t fi = 0; fi < currentClass->numFunctions(); ++fi)
		{
			results.push_back({currentClass->getFunction(fi), currentClass});
		}

		currentClass = currentClass->getBase();
	}

	return results;
}

}// end namespace ph::sdl
