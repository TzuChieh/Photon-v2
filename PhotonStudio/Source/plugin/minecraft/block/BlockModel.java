package plugin.minecraft.block;

import util.Vector2f;

public class BlockModel
{
	private String m_modelId;
	private int    m_rotDegreesX;
	private int    m_rotDegreesY;
	
	public BlockModel(String modelId)
	{
		m_modelId     = modelId;
		m_rotDegreesX = 0;
		m_rotDegreesY = 0;
	}
	
	public String getModelId()
	{
		return m_modelId;
	}
	
	public Vector2f getRotationDegrees()
	{
		return new Vector2f(m_rotDegreesX, m_rotDegreesY);
	}
	
	public void setRotationDegrees(int x, int y)
	{
		m_rotDegreesX = x;
		m_rotDegreesY = y;
	}
	
	@Override
	public String toString()
	{
		return "Model: " + m_modelId + ", xy rot: " + getRotationDegrees();
	}
}
