#pragma once

namespace ph
{

/*! @brief Status of a rendering region.
Note that when adding new entries, the order is important: later entries will take precedence when
multiple regions are merged together. For example, merging `Invalid` and `Updating` regions will result
in an `Updating` region.
*/
enum class ERegionStatus
{
	Invalid,
	Finished,
	Updating
};

}// end namespace ph
