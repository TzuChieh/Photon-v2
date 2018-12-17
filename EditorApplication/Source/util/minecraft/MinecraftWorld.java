package util.minecraft;

import util.Vector3f;

public class MinecraftWorld
{
	private Vector3f    m_viewpoint;
	private TerrainData m_terrain;
	
	public MinecraftWorld()
	{
		m_viewpoint = new Vector3f(0);
		m_terrain   = null;
	}
	
	public void setViewpoint(Vector3f viewpoint)
	{
		m_viewpoint = viewpoint;
	}
	
	public void setTerrain(TerrainData terrain)
	{
		m_terrain = terrain;
	}
	
	private RegionData getCurrentRegion(Vector3f viewpoint)
	{
		int regionX = (int)viewpoint.x / RegionData.SIZE_X;
		int regionZ = (int)viewpoint.z / RegionData.SIZE_Z;
		
		return m_terrain.getRegion(regionX, regionZ);
	}
}
