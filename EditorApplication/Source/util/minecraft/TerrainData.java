package util.minecraft;

import java.util.ArrayList;
import java.util.List;

import jsdl.CuboidGeometryCreator;
import jsdl.MatteOpaqueMaterialCreator;
import jsdl.ModelActorCreator;
import jsdl.SDLCommand;
import jsdl.SDLGeometry;
import jsdl.SDLMaterial;
import jsdl.SDLVector3;

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
	
	public List<SDLCommand> genSDLCommands()
	{
		List<SDLCommand> commands = new ArrayList<>();
		
		String materialName = "mat";
		MatteOpaqueMaterialCreator material = new MatteOpaqueMaterialCreator();
		material.setAlbedo(new SDLVector3(0.5f, 0.5f, 0.5f));
		material.setDataName(materialName);
		commands.add(material);
		
		for(RegionData region : m_regions)
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
						
						for(int y = 0; y < SectionData.SIZE_Y; ++y)
						{
							for(int z = 0; z < SectionData.SIZE_Z; ++z)
							{
								for(int x = 0; x < SectionData.SIZE_X; ++x)
								{
									String blockIdName = section.getBlockIdName(x, y, z);
									
									if(!blockIdName.equals("minecraft:air"))
									{
										String cubeName = chunkX + "," + chunkZ + "," + s + "," + x + "," + y + "," + z;
										
										CuboidGeometryCreator cube = new CuboidGeometryCreator();
										cube.setDataName(cubeName);
										
										int coordX = chunkX * 16 + x;
										int coordY = s * 16 + y;
										int coordZ = chunkZ * 16 + z;
										
										cube.setMinVertex(new SDLVector3(coordX, coordY, coordZ));
										cube.setMaxVertex(new SDLVector3(coordX + 1, coordY + 1, coordZ + 1));
										
										commands.add(cube);
										
										ModelActorCreator actor = new ModelActorCreator();
										actor.setDataName("actor:" + cubeName);
										actor.setMaterial(new SDLMaterial(materialName));
										actor.setGeometry(new SDLGeometry(cubeName));
										
										commands.add(actor);
									}
									
//									if(blockIdName.equals("minecraft:air"))
//									{
//										System.err.println("air @ (" + x + ", " + y + ", " + z + ")");
//									}
								}
							}
						}
					}
				}
			}
		}
		
		return commands;
	}
}
