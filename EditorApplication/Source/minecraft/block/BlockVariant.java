package minecraft.block;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

public class BlockVariant implements Iterable<BlockModel>
{
	private List<BlockModel> m_models;
	
	public BlockVariant()
	{
		m_models = new ArrayList<BlockModel>();
	}
	
	public boolean hasSingleModel()
	{
		return m_models.size() == 1;
	}
	
	public BlockModel getSingleModel()
	{
		return m_models.get(0);
	}
	
	public void addModel(BlockModel model)
	{
		m_models.add(model);
	}
	
	@Override
	public String toString()
	{
		return m_models.toString();
	}

	@Override
	public Iterator<BlockModel> iterator()
	{
		return m_models.iterator();
	}
}
