package plugin.minecraft;

import java.awt.Rectangle;
import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import javax.imageio.ImageIO;

import jsdl.CuboidGeometryCreator;
import jsdl.GeometrySoupGeometryAdd;
import jsdl.GeometrySoupGeometryAddTransformed;
import jsdl.GeometrySoupGeometryCreator;
import jsdl.LdrPictureImageCreator;
import jsdl.MatteOpaqueMaterialCreator;
import jsdl.PhantomModelActorCreator;
import jsdl.SDLGeometry;
import jsdl.SDLImage;
import jsdl.SDLMaterial;
import jsdl.SDLQuaternion;
import jsdl.SDLReal;
import jsdl.SDLString;
import jsdl.SDLVector3;
import plugin.minecraft.block.Block;
import plugin.minecraft.block.BlockData;
import plugin.minecraft.block.BlockModel;
import plugin.minecraft.parser.BlockParser;
import plugin.minecraft.parser.ModelParser;
import util.AABB2D;
import util.SDLConsole;
import util.Vector2f;
import util.Vector3f;
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
		return m_blockIdToActorName.getOrDefault(blockId, INVALID_BLOCK_ACTOR_NAME);
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
//		System.err.println("adding block " + block);
		
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
			
			// DEBUG
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
			for(int i = 0; i < blockEntry.getValue().numBlocks(); ++i)
			{
				Block block = blockEntry.getValue().getBlock(i);
				String blockName = blockEntry.getKey() + "_" + i;
				String blockActorName = genBlockActor(blockName, block, out_console);
				assert(blockActorName != null);
				
				m_blockIdToActorName.put(blockEntry.getKey(), blockActorName);
			}
		}
	}
	
	private String genBlockActor(String blockName, Block block, SDLConsole out_console)
	{
		List<BlockModel> blockModels = block.getModels();
		for(int i = 0; i < blockModels.size(); ++i)
		{
			BlockModel blockModel = blockModels.get(i);
			
			ModelData model = m_models.get(blockModel.getModelId());
			Set<String> textureIds = model.getRequiredTextures();
			
			TextureAtlas atlas = new TextureAtlas();
			for(String textureId : textureIds)
			{
				BufferedImage texture = m_textures.get(textureId);
				atlas.addImage(texture, textureId);
			}
			
			String modelName = blockName + "_model_" + i;
			String geometryName = genModelGeometry(modelName, model, atlas, out_console);
			
			// HACK
			LdrPictureImageCreator texture = new LdrPictureImageCreator();
			String textureName = modelName.replace(':', '_') + "_texture_" + i;
			texture.setDataName(textureName);
			try {
				File outputFile = new File("./test/" + textureName + ".png");
				outputFile.getParentFile().mkdirs();
				
				BufferedImage image = atlas.genImage();
				if(image != null)
				{
					ImageIO.write(image, "png", outputFile);
				}
				
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			texture.setImage(new SDLString("test/" + textureName + ".png"));
			if(!model.getRequiredTextures().isEmpty())
			{
				out_console.queue(texture);
			}
			
			String materialName = blockName + "_material_" + i;
			MatteOpaqueMaterialCreator material = new MatteOpaqueMaterialCreator();
			if(!model.getRequiredTextures().isEmpty())
			{
				material.setAlbedo(new SDLImage(textureName));
			}
			else
			{
				material.setAlbedo(new SDLVector3(0.5f, 0.5f, 0.5f));
			}
			material.setDataName(materialName);
			out_console.queue(material);
			
			PhantomModelActorCreator actor = new PhantomModelActorCreator();
			String actorName = blockName + "_actor_" + i;
			actor.setDataName(actorName);
			actor.setName(new SDLString(actorName));
			actor.setMaterial(new SDLMaterial(materialName));
			actor.setGeometry(new SDLGeometry(geometryName));
			out_console.queue(actor);
			
			// HACK
			return actorName;
		}
		
		return null;
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
	
	private String genModelGeometry(String modelName, ModelData model, TextureAtlas texture, SDLConsole out_console)
	{
		List<CuboidElement> elements = model.getElements();
		if(elements.isEmpty())
		{
			System.err.println("model: " + modelName);
		}
		
		GeometrySoupGeometryCreator geometry = new GeometrySoupGeometryCreator();
		String geometryName = modelName + "_geometry";
		geometry.setDataName(geometryName);
		out_console.queue(geometry);
		
		for(int i = 0; i < elements.size(); ++i)
		{
			CuboidElement element = elements.get(i);
			Vector3f minVertex = new Vector3f(element.getMinVertex());
			Vector3f maxVertex = new Vector3f(element.getMaxVertex());
			
			// to make planar geometry have at least some thickness
			Vector3f diagonal = maxVertex.sub(minVertex).maxLocal(new Vector3f(0.01f));
			maxVertex = minVertex.add(diagonal);
			
			CuboidGeometryCreator cuboid = new CuboidGeometryCreator();
			String cuboidName = modelName + "_element_" + i;
			cuboid.setDataName(cuboidName);
			cuboid.setMinVertex(new SDLVector3(minVertex.x, minVertex.y, minVertex.z));
			cuboid.setMaxVertex(new SDLVector3(maxVertex.x, maxVertex.y, maxVertex.z));
			for(EFacing facing : EFacing.values())
			{
				Face face = element.getFace(facing);
				if(face == null)
				{
					continue;
				}
				
				String faceTextureId = model.getTextureAssignment(face.getTextureVariable());
				
				AABB2D pixelCoord = new AABB2D();
				pixelCoord.min = face.getUVMin();
				pixelCoord.max = face.getUVMax();
				AABB2D pixelUV = texture.getNormalizedCoord(faceTextureId, pixelCoord);
				
				SDLQuaternion uv = new SDLQuaternion(pixelUV.min.x, pixelUV.min.y, pixelUV.max.x, pixelUV.max.y);
				switch(facing)
				{
				case NORTH: cuboid.setNzFaceUv(uv); break;
				case SOUTH: cuboid.setPzFaceUv(uv); break;
				case WEST:  cuboid.setNxFaceUv(uv); break;
				case EAST:  cuboid.setPxFaceUv(uv); break;
				case DOWN:  cuboid.setNyFaceUv(uv); break;
				case UP:    cuboid.setPyFaceUv(uv); break;
				}
			}
			out_console.queue(cuboid);
			
			GeometrySoupGeometryAddTransformed addToSoup = new GeometrySoupGeometryAddTransformed();
			addToSoup.setTargetName(geometryName);
			addToSoup.setGeometry(new SDLGeometry(cuboidName));
			addToSoup.setTranslation(element.getRotOrigin().toSDL());
			addToSoup.setRotationAxis(element.getRotAxis().toSDL());
			addToSoup.setRotationDegrees(new SDLReal(element.getRotDegrees()));
			out_console.queue(addToSoup);
		}
		
		return geometryName;
	}
}
