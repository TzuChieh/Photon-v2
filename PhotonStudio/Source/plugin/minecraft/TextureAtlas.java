package plugin.minecraft;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.Rectangle;
import java.awt.image.BufferedImage;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.sun.javafx.iio.ImageStorage;

import util.AABB2D;
import util.Vector2f;

public class TextureAtlas
{
	private List<BufferedImage> m_images;
	private Map<String, Integer> m_nameToIndex;
	
	public TextureAtlas()
	{
		m_images = new ArrayList<>();
		m_nameToIndex = new HashMap<>();
	}
	
	public void addImage(BufferedImage image)
	{
		addImage(image, null);
	}
	
	public void addImage(BufferedImage image, String name)
	{
		m_images.add(image);
		
		if(name != null)
		{
			m_nameToIndex.put(name, m_images.size() - 1);
		}
	}
	
	public int numImages()
	{
		return m_images.size();
	}
	
	public boolean isEmpty()
	{
		return m_images.isEmpty();
	}
	
	public AABB2D getNormalizedCoord(String imageName, AABB2D region)
	{
		return getNormalizedCoord(m_nameToIndex.get(imageName), region);
	}
	
	/**
	 * Gets normalized texture coordinates with respect to the atlas (with 
	 * lower-left corner being the origin).
	 * @param imageIndex index of the target image in the atlas
	 * @param region pixel coordinates in the target image that is going to 
	 * be normalized; origin is on the lower-left corner
	 * @return normalized texture coordinates in the atlas
	 */
	public AABB2D getNormalizedCoord(int imageIndex, AABB2D region)
	{
		int totalWidth = 0;
		int totalHeight = 0;
		int xOffset = 0;
		for(int i = 0; i < m_images.size(); ++i)
		{
			BufferedImage image = m_images.get(i);
			
			totalWidth += image.getWidth();
			totalHeight = Math.max(totalHeight, image.getHeight());
			xOffset += i < imageIndex ? image.getWidth() : 0;
		}
		
		AABB2D uv = new AABB2D();
		uv.min.x = (float)(xOffset + region.min.x) / (float)totalWidth;
		uv.min.y = (float)(region.min.y) / (float)totalHeight;
		uv.max.x = (float)(xOffset + region.max.x) / (float)totalWidth;
		uv.max.y = (float)(region.max.y) / (float)totalHeight;
		
		return uv;
	}
	
	public BufferedImage genImage()
	{
		if(m_images.isEmpty())
		{
			return null;
		}
		
		BufferedImage result = m_images.get(0);
		for(int i = 1; i < m_images.size(); ++i)
		{
			BufferedImage added = m_images.get(i);
			
			int newWidth  = result.getWidth() + added.getWidth();
			int newHeight = Math.max(result.getHeight(), added.getHeight());
			BufferedImage newImage = new BufferedImage(newWidth, newHeight, BufferedImage.TYPE_INT_ARGB);
			
			Graphics2D g2 = newImage.createGraphics();
			
			// fill background
			Color oldColor = g2.getColor();
			g2.setPaint(Color.WHITE);
			g2.fillRect(0, 0, newWidth, newHeight);
			g2.setColor(oldColor);
			
			// draw two images into the new image
			g2.drawImage(result, null, 0, newHeight - result.getHeight());
			g2.drawImage(added, null, result.getWidth(), newHeight - added.getHeight());
			g2.dispose();
			
			result = newImage;
		}
		return result;
	}
}
