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

}// end namespace ph
