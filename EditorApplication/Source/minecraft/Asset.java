package minecraft;

import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import javax.imageio.ImageIO;

import jsdl.CuboidGeometryCreator;
import jsdl.LdrPictureImageCreator;
import jsdl.MatteOpaqueMaterialCreator;
import jsdl.ModelActorCreator;
import jsdl.PhantomModelActorCreator;
import jsdl.SDLGeometry;
import jsdl.SDLImage;
import jsdl.SDLMaterial;
import jsdl.SDLString;
import jsdl.SDLVector3;
import minecraft.block.BlockData;
import minecraft.block.Block;
import minecraft.parser.BlockParser;
import minecraft.parser.ModelParser;
import util.SDLConsole;
import util.Vector3i;

public class Asset
{
	private static final String INVALID_BLOCK_ACTOR_NAME = "invalid_asset_block";
	
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
//		System.err.println("adding model " + model);
		
		assert(modelId != null);
		
		if(model != null)
		{
			m_models.put(modelId, model);
		}
	}
	
	public void add(String textureId, BufferedImage image)
	{
//		System.err.println("adding texture " + textureId);
		
		assert(textureId != null);
		
		if(image != null)
		{
			m_textures.put(textureId, image);
		}
	}
	
	public void add(String blockId, BlockData block)
	{
		System.err.println("adding block " + block);
		
		assert(blockId != null);
		
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
		genInvalidBlockActor(out_console);
		
		for(Map.Entry<String, BlockData> blockEntry : m_blocks.entrySet())
		{
			Block block = blockEntry.getValue().getVariant("");
			if(block == null)
			{
				for(Block ttt : blockEntry.getValue().getVariants())
				{
					block = ttt;
					break;
				}
			}
			
			String blockActorName = genBlockActor(blockEntry.getKey(), block, out_console);
			assert(blockActorName != null);
			
			m_blockIdToActorName.put(blockEntry.getKey(), blockActorName);
		}
	}
	
	private String genBlockActor(String blockId, Block block, SDLConsole out_console)
	{
//		if(!block.hasSingleModel())
//		{
//			return INVALID_BLOCK_ACTOR_NAME;
//		}
		
		Vector3i blockPos = new Vector3i(0, 0, 0);
		String cubeName = blockId + "_geometry";
		
		CuboidGeometryCreator cube = new CuboidGeometryCreator();
		cube.setDataName(cubeName);
		cube.setMinVertex(new SDLVector3(blockPos.x, blockPos.y, blockPos.z));
		cube.setMaxVertex(new SDLVector3(blockPos.x + 1, blockPos.y + 1, blockPos.z + 1));
		
		out_console.queue(cube);
		
		// HACK
		LdrPictureImageCreator image = new LdrPictureImageCreator();
		image.setDataName(blockId + "_image");
		ModelData model = m_models.get(block.getSingleModel().getModelId());
		for(String textureId : model.getRequiredTextures())
		{
			BufferedImage texture = m_textures.get(textureId);
			try {
				File outputFile = new File("./test/" + textureId + ".png");
				outputFile.getParentFile().mkdirs();
				ImageIO.write(texture, "png", outputFile);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			image.setImage(new SDLString("test/" + textureId + ".png"));
			break;
		}
		if(!model.getRequiredTextures().isEmpty())
		{
			out_console.queue(image);
		}
		
		String materialName = blockId + "_material";
		MatteOpaqueMaterialCreator material = new MatteOpaqueMaterialCreator();
		
		if(!model.getRequiredTextures().isEmpty())
		{
			material.setAlbedo(new SDLImage(blockId + "_image"));
		}
		else
		{
			material.setAlbedo(new SDLVector3(0.5f, 0.5f, 0.5f));
		}
		
		material.setDataName(materialName);
		
		
		out_console.queue(material);
		
		PhantomModelActorCreator actor = new PhantomModelActorCreator();
		actor.setDataName(blockId);
		actor.setName(new SDLString(blockId));
		actor.setMaterial(new SDLMaterial(materialName));
		actor.setGeometry(new SDLGeometry(cubeName));
		
		out_console.queue(actor);
		
		return blockId;
	}
	
	private void genInvalidBlockActor(SDLConsole out_console)
	{
		Vector3i blockPos = new Vector3i(0, 0, 0);
		String cubeName = INVALID_BLOCK_ACTOR_NAME + "_geometry";
		
		CuboidGeometryCreator cube = new CuboidGeometryCreator();
		cube.setDataName(cubeName);
		cube.setMinVertex(new SDLVector3(blockPos.x, blockPos.y, blockPos.z));
		cube.setMaxVertex(new SDLVector3(blockPos.x + 1, blockPos.y + 1, blockPos.z + 1));
		
		out_console.queue(cube);
		
		String materialName = INVALID_BLOCK_ACTOR_NAME + "_material";
		MatteOpaqueMaterialCreator material = new MatteOpaqueMaterialCreator();
		material.setAlbedo(new SDLVector3(0.5f, 0.5f, 0.5f));
		material.setDataName(materialName);
		
		out_console.queue(material);
		
		PhantomModelActorCreator actor = new PhantomModelActorCreator();
		actor.setDataName(INVALID_BLOCK_ACTOR_NAME);
		actor.setName(new SDLString(INVALID_BLOCK_ACTOR_NAME));
		actor.setMaterial(new SDLMaterial(materialName));
		actor.setGeometry(new SDLGeometry(cubeName));
		
		out_console.queue(actor);
	}
}
