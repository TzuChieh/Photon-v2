package minecraft;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class ModelData
{
	private String              m_parentModelId;
	private List<CuboidElement> m_elements;
	private Map<String, String> m_textureAssignments;
	
	public ModelData()
	{
		m_parentModelId      = null;
		m_elements           = new ArrayList<CuboidElement>();
		m_textureAssignments = new HashMap<>();
	}
	
	public void addElement(CuboidElement element)
	{
		m_elements.add(element);
	}
	
	public void assignTexture(String variable, String textureIdOrVariable)
	{
		m_textureAssignments.put(variable, textureIdOrVariable);
	}
	
	public void setParentModel(String modelId)
	{
		m_parentModelId = modelId;
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
