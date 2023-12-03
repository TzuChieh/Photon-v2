#pragma once

namespace ph
{

enum class EFieldImportance
{
	/*! Value for the field may be omitted as the default value is usually 
	what the user want. 
	*/
	Optional,

	/*! Value for the field should be provided as the engine may not be able 
	to provide a suitable value by itself.
	*/
	NiceToHave,

	/*! Value for the field is necessary and the engine provided value is
	typically not what the user meant to have.
	*/
	Required
};

}// end namespace ph
