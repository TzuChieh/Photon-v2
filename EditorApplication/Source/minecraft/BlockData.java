package minecraft;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class BlockData
{
	private String              m_parentBlockId;
	private BlockData           m_parent;
	private List<CuboidElement> m_elements;
	private Map<String, String> m_textureAssignments;
	
	public BlockData()
	{
		m_parentBlockId      = null;
		m_parent             = null;
		m_elements           = new ArrayList<>();
		m_textureAssignments = new HashMap<>();
	}
	
	public String getParentBlockId()
	{
		return m_parentBlockId;
	}
	
	public void addElement(CuboidElement element)
	{
		m_elements.add(element);
	}
	
	public void assignTexture(String variable, String textureIdOrVariable)
	{
		m_textureAssignments.put(variable, textureIdOrVariable);
	}
	
	public void setParentModelId(String modelId)
	{
		m_parentBlockId = modelId;
	}
	
	public void setParent(BlockData block)
	{
		m_parent = block;
	}
	
	@Override
	public String toString()
	{
		return 
			"Block Data:\n" + 
			"\tparent: " + m_parentBlockId + "\n" +
			"\telements: " + m_elements + "\n" + 
			"\ttexture assignments: " + m_textureAssignments;
	}
}
