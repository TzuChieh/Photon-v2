package minecraft;

public class LevelMetadata
{
	private float m_spPlayerYawDegrees;
	private float m_spPlayerPitchDegrees;
	
	public LevelMetadata()
	{
		m_spPlayerYawDegrees   = 0.0f;
		m_spPlayerPitchDegrees = 0.0f;
	}
	
	public void setSpPlayerYawDegrees(float degrees)
	{
		m_spPlayerYawDegrees = degrees;
	}
	
	public void setSpPlayerPitchDegrees(float degrees)
	{
		m_spPlayerPitchDegrees = degrees;
	}
	
	
}
