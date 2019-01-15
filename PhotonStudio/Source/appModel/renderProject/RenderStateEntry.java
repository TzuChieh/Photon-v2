package appModel.renderProject;

public class RenderStateEntry
{
	private static final int INTEGER = 1;
	private static final int REAL    = 2;
	
	private String m_name;
	private int    m_index;
	private int    m_type;
	
	public static RenderStateEntry newInteger(String name, int index)
	{
		return new RenderStateEntry(name, index, INTEGER);
	}
	
	public static RenderStateEntry newReal(String name, int index)
	{
		return new RenderStateEntry(name, index, REAL);
	}
	
	private RenderStateEntry(String name, int index, int type)
	{
		m_name  = name;
		m_index = index;
		m_type  = type;
	}
	
	public String getName()
	{
		return m_name;
	}
	
	public int getIndex()
	{
		return m_index;
	}
	
	public boolean isInteger()
	{
		return m_type == INTEGER;
	}
	
	public boolean isReal()
	{
		return m_type == REAL;
	}
}
