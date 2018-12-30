// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2018-12-30 09:20:21.497464 

package jsdl;

public class UniformRandomSampleGeneratorCreator extends SDLCoreCommand
{
	@Override
	public String getFullType()
	{
		return "sample-generator(uniform-random)";
	}

	public void setSampleAmount(SDLInteger data)
	{
		setInput("sample-amount", data);
	}

}

