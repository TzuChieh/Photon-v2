package plugin.minecraft;

import jsdl.DomeActorCreator;
import jsdl.MatteOpaqueMaterialCreator;
import jsdl.SDLInteger;
import jsdl.SDLString;
import jsdl.SDLVector3;
import jsdl.SamplingRendererCreator;
import jsdl.StratifiedSampleGeneratorCreator;
import jsdl.TransformedInstanceActorCreator;
import jsdl.TransformedInstanceActorTranslate;
import util.SDLConsole;
import util.Vector3i;

public class MinecraftWorld
{
	private float   m_fovDegrees;
	private Terrain m_terrain;
	private Asset   m_asset;
	
	// TODO: a MC camera class
	
	public MinecraftWorld(Terrain terrain, Asset asset)
	{
		m_fovDegrees = 105.0f;
		m_terrain    = terrain;
		m_asset      = asset;
	}
	
	public void toSDL(SDLConsole out_console)
	{
		StratifiedSampleGeneratorCreator sampleGenerator = new StratifiedSampleGeneratorCreator();
		sampleGenerator.setSampleAmount(new SDLInteger(10000));
		out_console.queue(sampleGenerator);
		
		SamplingRendererCreator renderer = new SamplingRendererCreator();
		renderer.setWidth(new SDLInteger(960));
		renderer.setHeight(new SDLInteger(540));
//		renderer.setWidth(new SDLInteger(1920));
//		renderer.setHeight(new SDLInteger(1080));
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
		
		for(SectionUnit section : m_terrain)
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
						
						Vector3i blockPos = section.getCoord().add(new Vector3i(x, y, z));
						
						assert(blockIdName != null) : "block position: " + blockPos;
						
						if(!blockIdName.equals("minecraft:air"))
						{
							String actorName = "actor:" + blockPos.x + "_" + blockPos.y + "_" + blockPos.z;
							
							TransformedInstanceActorCreator actor = new TransformedInstanceActorCreator();
							actor.setDataName(actorName);
							
							String blockActorName = m_asset.getBlockActorName(blockIdName);
							assert(blockActorName != null) : "block ID: " + blockIdName;
							actor.setName(new SDLString(blockActorName));
							
							out_console.queue(actor);
							
							TransformedInstanceActorTranslate translation = new TransformedInstanceActorTranslate();
							translation.setTargetName(actorName);
							translation.setFactor(new SDLVector3(blockPos.x, blockPos.y, blockPos.z));
							
							out_console.queue(translation);
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
}
