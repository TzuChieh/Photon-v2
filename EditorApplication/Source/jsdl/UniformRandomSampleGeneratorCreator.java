// ========================================
// NOTE: THIS FILE CONTAINS GENERATED CODE 
//       DO NOT MODIFY                     
// ========================================
// last generated: 2018-12-09 18:41:05.555505 

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

