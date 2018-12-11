package util.minecraft;

public class CompoundTag extends NBTTag
{
	private NBTData m_data;
	
	public CompoundTag()
	{
		m_data = null;
	}
	
	@Override
	@SuppressWarnings("unchecked")
	public NBTData getPayload()
	{
		return m_data;
	}
	
	public void setData(NBTData data)
	{
		m_data = data;
	}
}
