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
	public String generate()
	{
		StringBuilder fragments = new StringBuilder();
		fragments.append(getPrefix() + ' ');
		fragments.append(getFullType() + ' ');
		fragments.append(getName() + '(');
		fragments.append("\"@" + m_targetName + "\") ");
		generateInputs(fragments);
		fragments.append('\n');
		return fragments.toString();
	}
	
	public void setTargetName(String name)
	{
		m_targetName = name;
	}
}
