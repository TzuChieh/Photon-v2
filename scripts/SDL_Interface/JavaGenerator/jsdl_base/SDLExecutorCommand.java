package jsdl;

public abstract class SDLExecutorCommand extends SDLCommand
{
	private String m_targetName;
	
	protected SDLExecutorCommand()
	{
		m_targetName = "";
	}
	
	public abstract String getName();
	
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
		out_sdlBuffer.append(getName());
		out_sdlBuffer.append('(');
		out_sdlBuffer.append("@\"" + m_targetName + "\") ");
		generateInputs(out_sdlBuffer);
		out_sdlBuffer.append('\n');
	}
	
	public void setTargetName(String name)
	{
		m_targetName = name;
	}
}
