package util.minecraft;

public class DoubleTag extends NBTTag
{
	private double m_value;
	
	public DoubleTag()
	{
		m_value = 0;
	}
	
	@Override
	@SuppressWarnings("unchecked")
	public Double getPayload()
	{
		return m_value;
	}
	
	public void setValue(double value)
	{
		m_value = value;
	}
}
