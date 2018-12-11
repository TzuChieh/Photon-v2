package util.minecraft;

public class IntArrayTag extends NBTTag
{
	private int[] m_array;
	
	public IntArrayTag()
	{
		m_array = null;
	}
	
	@Override
	@SuppressWarnings("unchecked")
	public int[] getPayload()
	{
		return m_array;
	}
	
	public void setArray(int[] array)
	{
		m_array = array;
	}
}
