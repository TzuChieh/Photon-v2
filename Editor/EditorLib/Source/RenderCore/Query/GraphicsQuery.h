#pragma once

#include <Common/assertion.h>
#include <Common/primitive_type.h>
#include <Utility/utility.h>

#include <cstddef>
#include <memory>
#include <utility>

namespace ph::editor
{

class GHIQuery;
class GraphicsContext;

enum EGHIQuery
{
	Once,
	AutoRetry
};

class GraphicsQuery final
{
public:
	template<typename Query, typename... Args>
	GraphicsQuery once(Args&&... args);

	template<typename Query, typename... Args>
	GraphicsQuery autoRetry(Args&&... args);

	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS_NO_DTOR(GraphicsQuery);

	GraphicsQuery(std::shared_ptr<GHIQuery> query, EGHIQuery mode);
	~GraphicsQuery();

	/*!
	@return Whether the query is finished.
	*/
	bool run(GraphicsContext& ctx);

	GHIQuery& get();
	const GHIQuery& get() const;
	GHIQuery* operator -> ();
	const GHIQuery* operator -> () const;

private:
	std::shared_ptr<GHIQuery> m_query;
	uint32 m_numRetries = 0;
	EGHIQuery m_mode = EGHIQuery::Once;
};

template<typename Query, typename... Args>
inline GraphicsQuery GraphicsQuery::once(Args&&... args)
{
	return GraphicsQuery(std::make_shared<Query>(std::forward<Args>(args)...), EGHIQuery::Once);
}

template<typename Query, typename... Args>
inline GraphicsQuery GraphicsQuery::autoRetry(Args&&... args)
{
	return GraphicsQuery(std::make_shared<Query>(std::forward<Args>(args)...), EGHIQuery::AutoRetry);
}

inline GHIQuery& GraphicsQuery::get()
{
	PH_ASSERT(m_query);
	return *m_query;
}

inline const GHIQuery& GraphicsQuery::get() const
{
	PH_ASSERT(m_query);
	return *m_query;
}

inline GHIQuery* GraphicsQuery::operator -> ()
{
	return &(get());
}

inline const GHIQuery* GraphicsQuery::operator -> () const
{
	return &(get());
}

}// end namespace ph::editor
