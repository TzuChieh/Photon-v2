package util.minecraft;

import java.util.List;

import jsdl.CuboidGeometryCreator;
import jsdl.DomeActorCreator;
import jsdl.MatteOpaqueMaterialCreator;
import jsdl.ModelActorCreator;
import jsdl.PinholeCameraCreator;
import jsdl.SDLGeometry;
import jsdl.SDLInteger;
import jsdl.SDLMaterial;
import jsdl.SDLReal;
import jsdl.SDLString;
import jsdl.SDLVector3;
import jsdl.SamplingRendererCreator;
import jsdl.StratifiedSampleGeneratorCreator;
import util.SDLConsole;
import util.Vector3f;
import util.Vector3i;

public class MinecraftWorld
{
	private Vector3f    m_viewpoint;
	private TerrainData m_terrain;
	
	public MinecraftWorld()
	{
		m_viewpoint = new Vector3f(0);
		m_terrain   = null;
	}
	
	public void toSDL(SDLConsole out_console)
	{
//		SectionData.addBlockingBlock("minecraft:dirt");
//		SectionData.addBlockingBlock("minecraft:bedrock");
//		SectionData.addBlockingBlock("minecraft:grass_block");
		
		PinholeCameraCreator camera = new PinholeCameraCreator();
		camera.setFovDegree(new SDLReal(50.0f));
		camera.setPosition(new SDLVector3(60, 15, 240));
		camera.setDirection(new SDLVector3(1, -0.5f, 3));
		camera.setUpAxis(new SDLVector3(0, 1, 0));
		out_console.queue(camera);
		
		StratifiedSampleGeneratorCreator sampleGenerator = new StratifiedSampleGeneratorCreator();
		sampleGenerator.setSampleAmount(new SDLInteger(10000));
		out_console.queue(sampleGenerator);
		
		SamplingRendererCreator renderer = new SamplingRendererCreator();
		renderer.setWidth(new SDLInteger(960));
		renderer.setHeight(new SDLInteger(540));
		renderer.setFilterName(new SDLString("gaussian"));
		renderer.setEstimator(new SDLString("bneept"));
		out_console.queue(renderer);
		
		DomeActorCreator dome = new DomeActorCreator();
		dome.setDataName("envmap");
		dome.setEnvMap(new SDLString("spruit_sunrise_2k.hdr"));
		out_console.queue(dome);
		
		String materialName = "mat";
		MatteOpaqueMaterialCreator material = new MatteOpaqueMaterialCreator();
		material.setAlbedo(new SDLVector3(0.5f, 0.5f, 0.5f));
		material.setDataName(materialName);
		out_console.queue(material);
		
		List<SectionUnit> sections = m_terrain.getAllSections();
		for(SectionUnit section : sections)
		{
			System.err.println("generating... " + section);
			
			for(int y = 0; y < SectionData.SIZE_Y; ++y)
			{
				for(int z = 0; z < SectionData.SIZE_Z; ++z)
				{
					for(int x = 0; x < SectionData.SIZE_X; ++x)
					{
						SectionData sectionData = section.getData();
						String blockIdName = sectionData.getBlockIdName(x, y, z);
						
						if(!blockIdName.equals("minecraft:air"))
						{
							Vector3i blockPos = section.getCoord().add(new Vector3i(x, y, z));
							String cubeName = blockPos.toString();
							
							CuboidGeometryCreator cube = new CuboidGeometryCreator();
							cube.setDataName(cubeName);
							
							cube.setMinVertex(new SDLVector3(blockPos.x, blockPos.y, blockPos.z));
							cube.setMaxVertex(new SDLVector3(blockPos.x + 1, blockPos.y + 1, blockPos.z + 1));
							
							out_console.queue(cube);
							
							ModelActorCreator actor = new ModelActorCreator();
							actor.setDataName("actor:" + cubeName);
							actor.setMaterial(new SDLMaterial(materialName));
							actor.setGeometry(new SDLGeometry(cubeName));
							
							out_console.queue(actor);
						}
					}
				}
			}
		}
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
