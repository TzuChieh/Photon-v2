package minecraft;

import java.awt.image.BufferedImage;
import java.nio.file.Path;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class Asset
{
	private Map<String, BlockData>     m_blocks;
	private Map<String, BufferedImage> m_textures;
	
	public Asset()
	{
		m_blocks   = new HashMap<>();
		m_textures = new HashMap<>();
	}
	
	public void add(String blockId, BlockData block)
	{
		m_blocks.put(blockId, block);
	}
	
	public void add(String textureId, BufferedImage image)
	{
		m_textures.put(textureId, image);
	}
	
	public void loadBlocks(Path blockStorage, List<String> blockIds)
	{
		// TODO
	}
}
