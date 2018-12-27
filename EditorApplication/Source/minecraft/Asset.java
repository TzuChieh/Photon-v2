package minecraft;

import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;

import javax.imageio.ImageIO;

import minecraft.parser.ModelParser;

public class Asset
{
	private Map<String, ModelData>     m_models;
	private Map<String, BufferedImage> m_textures;
	
	public Asset()
	{
		m_models   = new HashMap<>();
		m_textures = new HashMap<>();
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
		if(image != null)
		{
			m_textures.put(textureId, image);
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
	
	public void loadTextures(Path textureStorage, Set<String> textureIds)
	{
		for(String textureId : textureIds)
		{
			Path texturePath = textureStorage.resolve(textureId + ".png");
			try
			{
				byte[] textureData = Files.readAllBytes(texturePath);
				
				ByteArrayInputStream rawData = new ByteArrayInputStream(textureData);
				BufferedImage texture = ImageIO.read(rawData);
				m_textures.put(textureId, texture);
			}
			catch(IOException e)
			{
				System.err.println("error loading texture with ID = " + textureId);
				e.printStackTrace();
			}
		}
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
}
