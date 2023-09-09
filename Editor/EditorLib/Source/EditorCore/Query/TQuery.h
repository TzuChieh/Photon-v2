#pragma once

#include "EditorCore/Query/fwd.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>
#include <Utility/utility.h>
#include <Utility/traits.h>

#include <cstddef>
#include <memory>
#include <utility>

namespace ph::editor
{

template<typename Target>
class TQuery final
{
public:
	using Performer = TQueryPerformer<Target>;

	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS_NO_DTOR(TQuery);

	TQuery(std::shared_ptr<Performer> performer, EQuery mode);
	~TQuery();

	/*!
	@return Whether the query is finished.
	*/
	bool run(Target& target);

	Performer& get();
	const Performer& get() const;
	Performer* operator -> ();
	const Performer* operator -> () const;

	template<CDerived<Performer> Performer, typename... Args>
	auto once(Args&&... args) -> TQuery
	{
		return TQuery(std::make_shared<Performer>(std::forward<Args>(args)...), EQuery::Once);
	}

	template<CDerived<Performer> Performer, typename... Args>
	auto autoRetry(Args&&... args) -> TQuery
	{
		return TQuery(std::make_shared<Performer>(std::forward<Args>(args)...), EQuery::AutoRetry);
	}

private:
	std::shared_ptr<Performer> m_performer;
	uint32 m_numRetries = 0;
	EQuery m_mode = EQuery::Once;
};

template<typename Target>
inline auto TQuery<Target>::get()
-> Performer&
{
	PH_ASSERT(m_performer);
	return *m_performer;
}

template<typename Target>
inline auto TQuery<Target>::get() const
-> const Performer&
{
	PH_ASSERT(m_performer);
	return *m_performer;
}

template<typename Target>
inline auto TQuery<Target>::operator -> ()
-> Performer*
{
	return &(get());
}

template<typename Target>
inline auto TQuery<Target>::operator -> () const
-> const Performer*
{
	return &(get());
}

}// end namespace ph::editor

#include "EditorCore/Query/TQuery.ipp"
