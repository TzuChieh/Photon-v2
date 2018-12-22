package minecraft;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class ModelData
{
	private String              m_parentModelId;
	private ModelData           m_parent;
	private List<CuboidElement> m_elements;
	private Map<String, String> m_textureAssignments;
	
	public ModelData()
	{
		m_parentModelId      = null;
		m_parent             = null;
		m_elements           = new ArrayList<>();
		m_textureAssignments = new HashMap<>();
	}
	
	public String getParentModelId()
	{
		return m_parentModelId;
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
		m_parentModelId = modelId;
	}
	
	public void setParent(ModelData model)
	{
		m_parent = model;
	}
	
	@Override
	public String toString()
	{
		return 
			"Model Data:\n" + 
			"\tparent: " + m_parentModelId + "\n" +
			"\telements: " + m_elements + "\n" + 
			"\ttexture assignments: " + m_textureAssignments;
	}
}
