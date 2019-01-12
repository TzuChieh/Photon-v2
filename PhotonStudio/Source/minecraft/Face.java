package minecraft;

import util.Vector2f;

public class Face
{
	private Vector2f m_uvMin;
	private Vector2f m_uvMax;
	private String   m_textureVariable;
	
	public Face()
	{
		m_uvMin           = null;
		m_uvMax           = null;
		m_textureVariable = null;
	}
	
	public Vector2f getUVMin()
	{
		return m_uvMin;
	}
	
	public Vector2f getUVMax()
	{
		return m_uvMax;
	}
	
	public String getTextureVariable()
	{
		return m_textureVariable;
	}
	
	public boolean hasTexture()
	{
		return m_textureVariable != null;
	}
	
	public boolean hasUV()
	{
		return m_uvMin != null && m_uvMax != null;
	}
	
	public void setUVMin(float u1, float v1)
	{
		m_uvMin = new Vector2f(u1, v1);
	}
	
	public void setUVMax(float u2, float v2)
	{
		m_uvMax = new Vector2f(u2, v2);
	}
	
	public void setTextureVariable(String variable)
	{
		m_textureVariable = variable;
	}
	
//	public void rotateTexture(int degrees)
//	{
//		switch(degrees)
//		{
//		case 0:
//			// nothing to permute
//			break;
//			
//		}
//	}
	
	@Override
	public String toString()
	{
		return "Face: texture variable = " + m_textureVariable;
	}
}
