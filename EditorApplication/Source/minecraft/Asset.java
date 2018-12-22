package minecraft;

import java.awt.image.BufferedImage;
import java.util.HashMap;
import java.util.Map;

public class Asset
{
	private Map<String, ModelData>     m_models;
	private Map<String, BufferedImage> m_textures;
	
	public Asset()
	{
		m_models   = new HashMap<>();
		m_textures = new HashMap<>();
	}
	
//	public ModelData getModel(String modelId)
//	{
//		return m_models.get(modelId);
//	}
//	
//	public BufferedImage getTexture(String textureId)
//	{
//		return m_textures.get(textureId);
//	}
	
	public void add(String modelId, ModelData model)
	{
		m_models.put(modelId, model);
	}
	
	public void add(String textureId, BufferedImage image)
	{
		m_textures.put(textureId, image);
	}
}
