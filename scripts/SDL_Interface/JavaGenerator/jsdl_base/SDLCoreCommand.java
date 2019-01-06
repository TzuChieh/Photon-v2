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
	public void generate(StringBuilder out_sdlBuffer)
	{
		out_sdlBuffer.append(getPrefix());
		out_sdlBuffer.append(' ');
		out_sdlBuffer.append(getFullType());
		out_sdlBuffer.append(' ');
		generateInputs(out_sdlBuffer);
		out_sdlBuffer.append('\n');
	}
}
