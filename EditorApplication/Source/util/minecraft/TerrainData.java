package util.minecraft;

import java.io.FileNotFoundException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import jsdl.CuboidGeometryCreator;
import jsdl.MatteOpaqueMaterialCreator;
import jsdl.ModelActorCreator;
import jsdl.SDLCommand;
import jsdl.SDLGeometry;
import jsdl.SDLMaterial;
import jsdl.SDLVector3;
import util.Vector3f;
import util.Vector3i;

public class TerrainData
{
	private Map<RegionCoord, RegionData> m_regions;
	
	public TerrainData()
	{
		m_regions = new HashMap<>();
	}
	
	public RegionData getRegion(int regionX, int regionZ)
	{
		return m_regions.get(new RegionCoord(regionX, regionZ));
	}
	
	public void addRegion(RegionData region)
	{
		m_regions.put(region.getRegionCoord(), region);
	}
	
	public List<SectionUnit> getAllSections()
	{
		List<SectionUnit> sections = new ArrayList<>();
		for(RegionData region : m_regions.values())
		{
			for(int chunkZ = 0; chunkZ < RegionData.NUM_CHUNKS_Z; ++chunkZ)
			{
				for(int chunkX = 0; chunkX < RegionData.NUM_CHUNKS_X; ++chunkX)
				{
					ChunkData chunk = region.getChunk(chunkX, chunkZ);
					if(chunk == null)
					{
						continue;
					}
					
					for(int s = 0; s < ChunkData.NUM_SECTIONS; ++s)
					{
						SectionData section = chunk.getSection(s);
						if(section == null)
						{
							continue;
						}
						
						final int x = region.getX() + chunkX * ChunkData.SIZE_X;
						final int y = region.getY() + s * ChunkData.SIZE_Y;
						final int z = region.getZ() + chunkZ * ChunkData.SIZE_Z;
						sections.add(new SectionUnit(new Vector3i(x, y, z), section));
					}
				}
			}
		}
		return sections;
	}
}
