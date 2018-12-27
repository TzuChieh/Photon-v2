package minecraft.block;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

public class BlockData
{
	private Map<String, BlockVariant> m_variants;
	
	public BlockData()
	{
		m_variants = new HashMap<>();
	}
	
	public BlockVariant getVariant(String name)
	{
		return m_variants.get(name);
	}
	
	public void addVariant(String name, BlockVariant variant)
	{
		m_variants.put(name, variant);
	}
	
	public Set<String> getRequiredModels()
	{
		Set<String> modelIds = new HashSet<>();
		for(BlockVariant variant : m_variants.values())
		{
			for(BlockModel model : variant)
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
