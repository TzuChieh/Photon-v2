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
	public String generate()
	{
		StringBuilder fragments = new StringBuilder();
		fragments.append(getPrefix() + ' ');
		fragments.append(getFullType() + ' ');
		fragments.append("\"@" + m_dataName + "\" ");
		generateInputs(fragments);
		fragments.append('\n');
		return fragments.toString();
	}
	
	public void setDataName(String name)
	{
		m_dataName = name;
	}
}
