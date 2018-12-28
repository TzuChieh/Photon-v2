package minecraft;

import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import javax.imageio.ImageIO;

import jsdl.CuboidGeometryCreator;
import jsdl.ModelActorCreator;
import jsdl.SDLGeometry;
import jsdl.SDLMaterial;
import jsdl.SDLVector3;
import minecraft.block.BlockData;
import minecraft.parser.BlockParser;
import minecraft.parser.ModelParser;
import util.SDLConsole;
import util.Vector3i;

public class Asset
{
	private Map<String, ModelData>     m_models;
	private Map<String, BufferedImage> m_textures;
	private Map<String, BlockData>     m_blocks;
	
	private Map<String, String> m_blockIdToActorName;
	
	public Asset()
	{
		m_models   = new HashMap<>();
		m_textures = new HashMap<>();
		m_blocks   = new HashMap<>();
		
		m_blockIdToActorName = new HashMap<>();
	}
	
	public String getBlockActorName(String blockId)
	{
		return m_blockIdToActorName.get(blockId);
	}
	
	public void add(String modelId, ModelData model)
	{
		System.err.println("adding model " + model);
		
		if(model != null)
		{
			m_models.put(modelId, model);
		}
	}
	
	public void add(String textureId, BufferedImage image)
	{
		System.err.println("adding texture " + textureId);
		
		if(image != null)
		{
			m_textures.put(textureId, image);
		}
	}
	
	public void add(String blockId, BlockData block)
	{
		System.err.println("adding block " + block);
		
		if(block != null)
		{
			m_blocks.put(blockId, block);
		}
	}
	
	public void loadModels(Path modelFolder, Set<String> modelIds)
	{
		ModelParser parser = new ModelParser();
		for(String modelId : modelIds)
		{
			loadModel(modelFolder, modelId, parser);
		}
	}
	
	public void loadTextures(Path textureFolder, Set<String> textureIds)
	{
		for(String textureId : textureIds)
		{
			if(m_textures.containsKey(textureId))
			{
				continue;
			}
			
			Path texturePath = textureFolder.resolve(textureId + ".png");
			try
			{
				byte[] textureData = Files.readAllBytes(texturePath);
				ByteArrayInputStream rawData = new ByteArrayInputStream(textureData);
				BufferedImage texture = ImageIO.read(rawData);
				add(textureId, texture);
			}
			catch(IOException e)
			{
				System.err.println("error loading texture " + texturePath);
				e.printStackTrace();
			}
		}
	}
	
	public void loadBlocks(Path blockFolder, Set<String> blockIds)
	{
		BlockParser parser = new BlockParser();
		for(String blockId : blockIds)
		{
			if(m_blocks.containsKey(blockId))
			{
				continue;
			}
			
			System.err.println("block " + blockId);
			
			// block ID is in the format <namespace>:<actual ID>, we need to 
			// remove the namespace for path resolving
			String actualId = blockId.substring(blockId.indexOf(':') + 1);
			Path blockPath = blockFolder.resolve(actualId + ".json");
			
			try
			{
				byte[] blockData = Files.readAllBytes(blockPath);
				ByteArrayInputStream rawData = new ByteArrayInputStream(blockData);
				BlockData block = parser.parse(rawData);
				add(blockId, block);
			}
			catch(IOException e)
			{
				System.err.println("error loading block " + blockPath);
				e.printStackTrace();
			}
		}
	}
	
	public void loadAllIdentified(Path modelFolder, Path textureFolder)
	{
		Set<String> requiredModels = new HashSet<>();
		for(BlockData block : m_blocks.values())
		{
			requiredModels.addAll(block.getRequiredModels());
		}
		loadModels(modelFolder, requiredModels);
		
		Set<String> requiredTextures = new HashSet<>();
		for(String modelId : requiredModels)
		{
			ModelData model = m_models.get(modelId);
			requiredTextures.addAll(model.getRequiredTextures());
		}
		loadTextures(textureFolder, requiredTextures);
	}
	
	private void loadModel(Path modelFolder, String modelId, ModelParser parser)
	{
		if(m_models.containsKey(modelId))
		{
			return;
		}
		
		Path modelPath = modelFolder.resolve(modelId + ".json");
		
		ModelData model = null;
		try
		{
			byte[] modelData = Files.readAllBytes(modelPath);
			ByteArrayInputStream rawData = new ByteArrayInputStream(modelData);
			model = parser.parse(rawData);
			add(modelId, model);
		}
		catch(IOException e)
		{
			System.err.println("error loading model " + modelPath);
			e.printStackTrace();
		}
		
		if(model != null && model.hasParent())
		{
			String parentModelId = model.getParentId();
			loadModel(modelFolder, parentModelId, parser);
			model.setParent(m_models.get(parentModelId));
		}
	}
	
	public void genBlockActors(SDLConsole out_console)
	{
		for(Map.Entry<String, BlockData> block : m_blocks.entrySet())
		{
//			String blockActorName = genBlockActor(block.getKey(), block.getValue(), out_console);
//			m_blockIdToActorName.put(block.getKey(), blockActorName);
		}
	}
	
//	private String genBlockActor(String blockId, BlockData blockData, SDLConsole out_console)
//	{
//		if(!blockIdName.equals("minecraft:air"))
//		{
//			Vector3i blockPos = section.getCoord().add(new Vector3i(x, y, z));
//			String cubeName = blockPos.toString();
//			
//			CuboidGeometryCreator cube = new CuboidGeometryCreator();
//			cube.setDataName(cubeName);
//			
//			cube.setMinVertex(new SDLVector3(blockPos.x, blockPos.y, blockPos.z));
//			cube.setMaxVertex(new SDLVector3(blockPos.x + 1, blockPos.y + 1, blockPos.z + 1));
//			
//			out_console.queue(cube);
//			
//			ModelActorCreator actor = new ModelActorCreator();
//			actor.setDataName("actor:" + cubeName);
//			actor.setMaterial(new SDLMaterial(materialName));
//			actor.setGeometry(new SDLGeometry(cubeName));
//			
//			out_console.queue(actor);
//		}
//		
//		return "";
//	}
}
