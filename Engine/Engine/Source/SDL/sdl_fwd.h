#pragma once

namespace ph
{

class ISdlInstantiable;
class SdlClass;
class SdlStruct;
class SdlStructFieldStump;
class SdlField;
class SdlFunction;
class SdlEnum;

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

enum ESdlClauseValue
{
	/*! Value of unspecified format. */
	General = 0,

	/*! Name of a single persistent target. */
	PersistentTargetName,

	/*! Name of a single cached target. */
	CachedTargetName
};

}// end namespace ph
