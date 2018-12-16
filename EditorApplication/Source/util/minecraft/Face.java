package util.minecraft;

import util.Vector2f;

public class Face
{
	private Vector2f m_uvMin;
	private Vector2f m_uvMax;
	private String   m_textureVariable;
	
	public Face()
	{
		m_uvMin           = new Vector2f();
		m_uvMax           = new Vector2f();
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
	
	public void setUVMin(float u1, float v1)
	{
		m_uvMin.x = u1;
		m_uvMin.y = v1;
	}
	
	public void setUVMax(float u2, float v2)
	{
		m_uvMax.x = u2;
		m_uvMax.y = v2;
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
}
