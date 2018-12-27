package minecraft;

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
import util.Vector2f;
import util.Vector3f;
import util.Vector3i;

public class MinecraftWorld
{
	private float    m_fovDegrees;
	private Terrain  m_terrain;
	private Asset    m_asset;
	private LevelMetadata m_levelMetadata;
	
	public MinecraftWorld(Terrain terrain, Asset asset)
	{
		m_fovDegrees    = 105.0f;
		m_terrain       = terrain;
		m_asset         = asset;
		m_levelMetadata = new LevelMetadata();
	}
	
	public void toSDL(SDLConsole out_console)
	{
//		SectionData.addTransparentBlock("minecraft:dirt");
//		SectionData.addTransparentBlock("minecraft:bedrock");
//		SectionData.addTransparentBlock("minecraft:grass_block");
//		SectionData.addTransparentBlock("minecraft:stone");
//		SectionData.addTransparentBlock("minecraft:stone:1");
//		SectionData.addTransparentBlock("minecraft:stone:2");
//		SectionData.addTransparentBlock("minecraft:stone:3");
//		SectionData.addTransparentBlock("minecraft:stone:4");
//		SectionData.addTransparentBlock("minecraft:stone:5");
//		SectionData.addTransparentBlock("minecraft:stone:6");
//		SectionData.addTransparentBlock("minecraft:grass");
//		SectionData.addTransparentBlock("minecraft:sand");
//		SectionData.addTransparentBlock("minecraft:coal_ore");
//		SectionData.addTransparentBlock("minecraft:iron_ore");
//		SectionData.addTransparentBlock("minecraft:gold_ore");
//		SectionData.addTransparentBlock("minecraft:sandstone");
//		SectionData.addTransparentBlock("minecraft:lava");
//		SectionData.addTransparentBlock("minecraft:cobblestone");
		SectionData.addTransparentBlock("minecraft:water");
		
//		SectionData.addTransparentBlock("minecraft:andesite");
//		SectionData.addTransparentBlock("minecraft:granite");
//		SectionData.addTransparentBlock("minecraft:diorite");
//		SectionData.addTransparentBlock("minecraft:granite");
//		SectionData.addTransparentBlock("minecraft:gravel");
		
		SectionData.addTransparentBlock("minecraft:air");
//		SectionData.addTransparentBlock("minecraft:snow");
		
		PinholeCameraCreator camera = new PinholeCameraCreator();
		camera.setFovDegree(new SDLReal(m_fovDegrees));
		
		Vector3f camPos = m_levelMetadata.getSpPlayerPosition();
		camera.setPosition(new SDLVector3(camPos.x, camPos.y, camPos.z));
		
		Vector2f yawPitch = m_levelMetadata.getSpPlayerYawPitchDegrees();
		camera.setYawDegrees(new SDLReal(yawPitch.x));
		camera.setPitchDegrees(new SDLReal(yawPitch.y));
		
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
		
		List<SectionUnit> sections = m_terrain.getReachableSections(camPos);
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
		}// end for each section
	}
	
	public void setFovDegrees(float fovDegrees)
	{
		m_fovDegrees = fovDegrees;
	}
	
	public void setLevelMetadata(LevelMetadata data)
	{
		m_levelMetadata = data;
	}
}
