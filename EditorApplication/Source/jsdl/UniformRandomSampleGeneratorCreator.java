// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2018-12-28 15:49:23.835500 

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

