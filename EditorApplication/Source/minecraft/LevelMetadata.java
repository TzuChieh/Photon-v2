package minecraft;

import util.Vector2f;
import util.Vector3f;

public class LevelMetadata
{
	private Vector3f m_spPlayerPosition;
	private float    m_spPlayerYawDegrees;
	private float    m_spPlayerPitchDegrees;
	
	public LevelMetadata()
	{
		m_spPlayerPosition     = new Vector3f(0);
		m_spPlayerYawDegrees   = 0.0f;
		m_spPlayerPitchDegrees = 0.0f;
	}
	
	public Vector3f getSpPlayerPosition()
	{
		return m_spPlayerPosition;
	}
	
	public Vector2f getSpPlayerYawPitchDegrees()
	{
		return new Vector2f(m_spPlayerYawDegrees, m_spPlayerPitchDegrees);
	}
	
	public void setSpPlayerPosition(Vector3f position)
	{
		m_spPlayerPosition = position;
	}
	
	public void setSpPlayerYawDegrees(float degrees)
	{
		// degrees is the amount of CW rotation around the Y axis
		// (due south is 0, and does not exceed 360 degrees)
		
		// converts to Photon's expectations
		float ccwDeg         = 360 - degrees;
		float dueNorthIs0Deg = ccwDeg - 180;
		float photonDeg      = dueNorthIs0Deg < 0 ? 180 + dueNorthIs0Deg : dueNorthIs0Deg;
		
		m_spPlayerYawDegrees = photonDeg;
	}
	
	public void setSpPlayerPitchDegrees(float degrees)
	{
		// degrees is the declination from the horizon in [-90, 90]
		// (horizontal is 0, positive values look downward)
		
		// converts to Photon's expectations
		float upIsPositiveDeg = -degrees;
		
		m_spPlayerPitchDegrees = upIsPositiveDeg;
	}
}
