package minecraft.block;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class BlockData
{
	private List<StateConditional> m_conditionals;
	private List<Block>            m_blocks;
	
	public BlockData()
	{
		m_conditionals = new ArrayList<>();
		m_blocks       = new ArrayList<>();
	}
	
	public Block getBlock(StateAggregate states)
	{
		for(int i = 0; i < m_conditionals.size(); ++i)
		{
			StateConditional conditional = m_conditionals.get(i);
			if(conditional.evaluate(states))
			{
				return m_blocks.get(i);
			}
		}
		return null;
	}
	
	public Block getBlock(int blockIndex)
	{
		return m_blocks.get(blockIndex);
	}
	
	public int numBlocks()
	{
		return m_blocks.size();
	}
	
	public void addBlock(StateConditional conditional, Block block)
	{
		m_conditionals.add(conditional);
		m_blocks.add(block);
	}
	
	public Set<String> getRequiredModels()
	{
		Set<String> modelIds = new HashSet<>();
		for(Block block : m_blocks)
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
		return "Block Data: conditionals = " + m_conditionals + ", models = " + m_blocks;
	}
}
