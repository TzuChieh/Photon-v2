package util.minecraft;

import java.util.ArrayList;
import java.util.List;

public class TerrainData
{
	private List<RegionData> m_regions;
	
	public TerrainData()
	{
		m_regions = new ArrayList<>();
	}
	
	public void addRegion(RegionData region)
	{
		m_regions.add(region);
	}
}
