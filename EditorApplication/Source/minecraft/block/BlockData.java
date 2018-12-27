package minecraft.block;

import java.util.HashMap;
import java.util.Map;

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
	
	@Override
	public String toString()
	{
		return "Block Data: " + m_variants;
	}
}
