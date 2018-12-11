package util.minecraft;

public class FloatTag extends NBTTag
{
	private float m_value;
	
	public FloatTag()
	{
		m_value = 0;
	}
	
	@Override
	@SuppressWarnings("unchecked")
	public Float getPayload()
	{
		return m_value;
	}
	
	public void setValue(float value)
	{
		m_value = value;
	}
}
