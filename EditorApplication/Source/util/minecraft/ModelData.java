package util.minecraft;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class ModelData
{
	private List<CubicElement>  m_elements;
	private Map<String, String> m_textureAssignments;
	
	public ModelData()
	{
		m_elements           = new ArrayList<CubicElement>();
		m_textureAssignments = new HashMap<>();
	}
	
	public void addElement(CubicElement element)
	{
		m_elements.add(element);
	}
	
	public void assignTexture(String variable, String textureId)
	{
		m_textureAssignments.put(variable, textureId);
	}
}
