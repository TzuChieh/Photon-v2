#pragma once

#include "EditorCore/Query/TQueryPerformer.h"

namespace ph::editor
{

template<typename Target>
inline TQueryPerformer<Target>::~TQueryPerformer() = default;

template<typename Target>
inline bool TQueryPerformer<Target>::isReady() const
{
	if(m_readyFlag.test(std::memory_order_relaxed))
	{
		if(m_readyFlag.test(std::memory_order_acquire))
		{
			return true;
		}
	}

	return false;
}

template<typename Target>
inline void TQueryPerformer<Target>::cancel()
{
	m_cancelFlag.test_and_set(std::memory_order_relaxed);
}

template<typename Target>
inline void TQueryPerformer<Target>::queryDone()
{
	m_readyFlag.test_and_set(std::memory_order_release);
}

template<typename Target>
inline bool TQueryPerformer<Target>::isCanceled() const
{
	return m_cancelFlag.test(std::memory_order_relaxed);
}

}// end namespace ph::editor
