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
	
	public void genSDLCommands()
	{
		SectionData.addBlockingBlock("minecraft:dirt");
		SectionData.addBlockingBlock("minecraft:bedrock");
		SectionData.addBlockingBlock("minecraft:grass_block");
		
		StringBuilder sdlBuffer = new StringBuilder();
		PrintWriter sdl = null;
		try {
			sdl = new PrintWriter("./mc.p2");
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		String materialName = "mat";
		MatteOpaqueMaterialCreator material = new MatteOpaqueMaterialCreator();
		material.setAlbedo(new SDLVector3(0.5f, 0.5f, 0.5f));
		material.setDataName(materialName);
		material.generate(sdlBuffer);
		
		for(RegionData region : m_regions.values())
		{
			for(int chunkZ = 0; chunkZ < RegionData.NUM_CHUNKS_Z; ++chunkZ)
			{
				for(int chunkX = 0; chunkX < RegionData.NUM_CHUNKS_X; ++chunkX)
				{
					System.err.println("generating chunk (" + chunkX + ", " + chunkZ + ")");
					
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
						
						FaceReachability reachability = section.determinReachability();
						if(reachability.isFullyReachable())
						{
							System.err.println(section);
						}
						System.err.println("section: " + s + ", " + reachability);
						
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
										
										cube.generate(sdlBuffer);
										
										ModelActorCreator actor = new ModelActorCreator();
										actor.setDataName("actor:" + cubeName);
										actor.setMaterial(new SDLMaterial(materialName));
										actor.setGeometry(new SDLGeometry(cubeName));
										
										actor.generate(sdlBuffer);
									}
									
//									if(blockIdName.equals("minecraft:air"))
//									{
//										System.err.println("air @ (" + x + ", " + y + ", " + z + ")");
//									}
								}
							}
						}
					}
					
					sdl.append(sdlBuffer);
					sdlBuffer.setLength(0);
				}
			}
		}
		
		sdl.close();
	}
}
