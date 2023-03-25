package jsdl;

public abstract class SDLCreatorCommand extends SDLCommand
{
	private String m_dataName;
	
	protected SDLCreatorCommand()
	{
		m_dataName = "";
	}
	
	@Override
	public abstract String getFullType();
	
	@Override
	public String getPrefix()
	{
		return "->";
	}

	@Override
	public void generate(StringBuilder out_sdlBuffer)
	{
		out_sdlBuffer.append(getPrefix());
		out_sdlBuffer.append(' ');
		out_sdlBuffer.append(getFullType());
		out_sdlBuffer.append(' ');
		out_sdlBuffer.append("\"" + m_dataName + "\" ");
		generateInputs(out_sdlBuffer);
		out_sdlBuffer.append('\n');
	}
	
	public void setDataName(String name)
	{
		m_dataName = name;
	}
}
