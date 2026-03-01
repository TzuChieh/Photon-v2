#pragma once

namespace ph
{

struct SdlInstantiated;
class ISdlInstantiable;
class SdlClass;
class SdlStruct;
class SdlField;
class SdlFunction;
class SdlEnum;

template<typename Owner>
class TSdlStructFieldStub;

template<typename EnumType>
class TSdlEnum;

class SdlInputContext;
class SdlInputClause;
class SdlInputClauses;
class SdlOutputContext;
class SdlOutputClause;
class SdlOutputClauses;
class SdlNamedOutputClauses;
class SdlDataPacketInterface;
class SdlCommandGenerator;
class SdlCommandParser;
class SdlDependencyResolver;
class SdlNativeData;

template<bool IS_CONST>
class TSdlAnyInstance;

using SdlConstInstance = TSdlAnyInstance<true>;
using SdlNonConstInstance = TSdlAnyInstance<false>;

class ISdlResource;
class ISdlReferenceGroup;
class ISdlDataPacketGroup;

/*! @brief Meta information for the value in a clause.
*/
enum ESdlClauseValue
{
	/*! Value of unspecified format. */
	General = 0,

	/*! Name of a single persistent target. */
	PersistentTargetName,

	/*! Name of a single cached target. */
	CachedTargetName
};

// Definer types

template<typename Def>
class TSdlClassDefiner;

template<typename Def>
class TSdlStructDefiner;

template<typename Def>
class TSdlFunctionDefiner;

// Concrete introspect types

template<typename ClassType, typename FieldSet>
class TSdlOwnerClass;

template<typename StructType, typename FieldSet>
class TSdlOwnerStruct;

template<typename MethodStruct, typename TargetType, typename FieldSet>
class TSdlOwnerMethod;

template<typename MethodStruct, typename FieldSet>
class TSdlOwnerStaticMethod;

}// end namespace ph
