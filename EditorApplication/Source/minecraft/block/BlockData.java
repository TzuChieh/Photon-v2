package minecraft.block;

import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

public class BlockData
{
	private Map<String, Block> m_variants;
	
	public BlockData()
	{
		m_variants = new HashMap<>();
	}
	
	public Block getVariant(String name)
	{
		return m_variants.get(name);
	}
	
	public Collection<Block> getVariants()
	{
		return m_variants.values();
	}
	
	public void addVariant(String name, Block block)
	{
		m_variants.put(name, block);
	}
	
	public Set<String> getRequiredModels()
	{
		Set<String> modelIds = new HashSet<>();
		for(Block block : m_variants.values())
		{
			for(BlockModel model : block)
			{
				modelIds.add(model.getModelId());
			}
		}
		return modelIds;
	}
	
	@Override
	public String toString()
	{
		return "Block Data: " + m_variants;
	}
}
