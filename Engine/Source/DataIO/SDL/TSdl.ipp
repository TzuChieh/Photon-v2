#include "DataIO/SDL/TSdl.h"

namespace ph
{

template<sdl::CIsResource T>
inline constexpr ETypeCategory TSdl<T>::getCategory()
{
	return sdl::category_of<T>();
}

template<sdl::CIsResource T>
inline std::shared_ptr<T> TSdl<T>::makeResource()
{
	// TODO
}

}// end namespace ph
