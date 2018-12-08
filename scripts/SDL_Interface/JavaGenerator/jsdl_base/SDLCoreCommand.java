package jsdl;

public abstract class SDLCoreCommand extends SDLCommand
{
	@Override
	public abstract String getFullType();
	
	@Override
	public String getPrefix()
	{
		return "##";
	}

	@Override
	public String generate()
	{
		StringBuilder fragments = new StringBuilder();
		fragments.append(getPrefix() + ' ');
		fragments.append(getFullType() + ' ');
		generateInputs(fragments);
		fragments.append('\n');
		return fragments.toString();
	}
}
