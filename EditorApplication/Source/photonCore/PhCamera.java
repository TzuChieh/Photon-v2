package photonCore;

import photonApi.LongRef;
import photonApi.Ph;

public final class PhCamera
{
	public static enum Type
	{
		PINHOLE(Ph.PH_PINHOLE_CAMERA_TYPE);
		
		private final int m_phValue;
		
		private Type(int phValue)
		{
			m_phValue = phValue;
		}
		
		public int getValue()
		{
			return m_phValue;
		}
	}
	
	protected long m_cameraId;
	
	public PhCamera(Type type)
	{
		LongRef cameraId = new LongRef();
		Ph.phCreateCamera(cameraId, type.getValue());
		m_cameraId = cameraId.m_value;
	}
	
	public void setPosition(float x, float y, float z)
	{
		Ph.phSetCameraPosition(m_cameraId, x, y, z);
	}
	
	public void setDirection(float x, float y, float z)
	{
		Ph.phSetCameraDirection(m_cameraId, x, y, z);
	}
	
	public void setFilm(PhFilm film)
	{
		Ph.phSetCameraFilm(m_cameraId, film.m_filmId);
	}
}
