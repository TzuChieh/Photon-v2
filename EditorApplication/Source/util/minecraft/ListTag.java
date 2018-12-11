package util.minecraft;

public class ListTag<T extends NBTTag> extends NBTTag
{
	private T[] m_array;
	
	public ListTag()
	{
		m_array = null;
	}
	
	@Override
	@SuppressWarnings("unchecked")
	public T[] getPayload()
	{
		return m_array;
	}
	
	public void setArray(T[] array)
	{
		m_array = array;
	}
}
