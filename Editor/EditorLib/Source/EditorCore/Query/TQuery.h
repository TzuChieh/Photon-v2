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

/*!
The basic release-acquire synchronization helper methods provided by query performer alone cannot
guarantee fully thread-safe behavior. Specifically, whether the data carried by the query object (
including the performer) is visible after passing to another thread is entirely depending on the
external synchronization done by the user. `TQuery` and `TQueryPerformer` simply provide helper
methods to "publish" query result across threads (in a thread-safe way).

`TConcurrentQueryManager` is another helper for dealing with the synchronization required when passing
query object across threads. It is fully thread-safe.
*/
template<typename Target, typename Performer = TQueryPerformer<Target>>
class TQuery final
{
public:
	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS_NO_DTOR(TQuery);

	TQuery(std::shared_ptr<Performer> performer, EQuery mode);

	/*! @brief Implicitly copy from a query object with derived performer.
	*/
	template<CDerived<Performer> DerivedPerformer>
	TQuery(const TQuery<Target, DerivedPerformer>& derivedQuery);

	/*! @brief Implicitly move from a query object with derived performer.
	*/
	template<CDerived<Performer> DerivedPerformer>
	TQuery(TQuery<Target, DerivedPerformer>&& derivedQuery);

	~TQuery();

	/*!
	@return Whether the query is finished.
	*/
	bool run(Target& target);

	/*! @brief Clear the underlying query performer.
	After this call, `isEmpty()` is true.
	*/
	void clear();

	bool isEmpty() const;

	/*! @brief Cancel the query.
	Does nothing if `isEmpty()` is true.
	*/
	void cancel();

	/*! @brief Whether the query is canceled.
	*/
	bool isCanceled() const;

	Performer& get();
	const Performer& get() const;
	Performer* operator -> ();
	const Performer* operator -> () const;

	template<CDerived<Performer> DerivedPerformer = Performer, typename... Args>
	static auto once(Args&&... args) -> TQuery<Target, DerivedPerformer>
	{
		return TQuery<Target, DerivedPerformer>(
			std::make_shared<DerivedPerformer>(std::forward<Args>(args)...), EQuery::Once);
	}

	template<CDerived<Performer> DerivedPerformer = Performer, typename... Args>
	static auto autoRetry(Args&&... args) -> TQuery<Target, DerivedPerformer>
	{
		return TQuery<Target, DerivedPerformer>(
			std::make_shared<DerivedPerformer>(std::forward<Args>(args)...), EQuery::AutoRetry);
	}

private:
	std::shared_ptr<Performer> m_performer;
	uint32 m_numRetries = 0;
	EQuery m_mode = EQuery::Once;

	// For implicit copy/move from derived performer.
	// (would be better if more constraints can be specified in the friend declaration)
	friend class TQuery;
};

template<typename Target, typename Performer>
inline void TQuery<Target, Performer>::clear()
{
	m_performer = nullptr;
}

template<typename Target, typename Performer>
inline bool TQuery<Target, Performer>::isEmpty() const
{
	return m_performer == nullptr;
}

template<typename Target, typename Performer>
inline auto TQuery<Target, Performer>::get()
-> Performer&
{
	PH_ASSERT(m_performer);
	return *m_performer;
}

template<typename Target, typename Performer>
inline auto TQuery<Target, Performer>::get() const
-> const Performer&
{
	PH_ASSERT(m_performer);
	return *m_performer;
}

template<typename Target, typename Performer>
inline auto TQuery<Target, Performer>::operator -> ()
-> Performer*
{
	return &(get());
}

template<typename Target, typename Performer>
inline auto TQuery<Target, Performer>::operator -> () const
-> const Performer*
{
	return &(get());
}

}// end namespace ph::editor

#include "EditorCore/Query/TQuery.ipp"
