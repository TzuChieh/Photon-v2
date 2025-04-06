#pragma once

namespace ph::editor
{

enum EQuery
{
	Once,
	AutoRetry
};

template<typename Target>
class TQueryPerformer;

template<typename Target, typename Performer>
class TQuery;

}// end namespace ph::editor
