package plugin.minecraft;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class ModelData
{
	private String              m_parentId;
	private ModelData           m_parent;
	private List<CuboidElement> m_elements;
	private Map<String, String> m_textureAssignments;
	
	public ModelData()
	{
		m_parentId           = null;
		m_parent             = null;
		m_elements           = new ArrayList<>();
		m_textureAssignments = new HashMap<>();
	}
	
	public String getParentId()
	{
		return m_parentId;
	}
	
	public boolean hasParent()
	{
		return m_parentId != null;
	}
	
	public String getTextureAssignment(String variable)
	{
		assert(isVariable(variable));
		
		// recursively search assignment from base to derived 
		String assigned = null;
		if(hasParent() && m_parent != null)
		{
			assigned = m_parent.getTextureAssignment(variable);
		}
		
		if(assigned == null)
		{
			assigned = m_textureAssignments.get(variable);
		}
		else if(isVariable(assigned))
		{
			assigned = m_textureAssignments.getOrDefault(assigned, assigned);
		}
		
		return assigned;
	}
	
	public List<CuboidElement> getElements()
	{
		// If a model contains element and has parent, it should override 
		// parent's element.
		
		if(!m_elements.isEmpty())
		{
			return m_elements;
		}
		else if(hasParent() && m_parent != null)
		{
			return m_parent.getElements();
		}
		else
		{
			return new ArrayList<CuboidElement>();
		}
	}
	
	public Set<String> getRequiredTextures()
	{
		Set<String> textureIds = new HashSet<>();
		for(CuboidElement element : getElements())
		{
			for(EFacing facing : EFacing.values())
			{
				Face face = element.getFace(facing);
				if(face == null)
				{
					continue;
				}
				
				String variable   = face.getTextureVariable();
				String assignment = getTextureAssignment(variable);
				if(assignment != null && !isVariable(assignment))
				{
					textureIds.add(assignment);
				}
				else
				{
					MCLogger.warn("cannot find assignment for texture variable " + variable);
				}
			}
		}
		return textureIds;
	}
	
	public void addElement(CuboidElement element)
	{
		m_elements.add(element);
	}
	
	public void assignTexture(String variable, String textureIdOrVariable)
	{
		assert(isVariable(variable));
		
		m_textureAssignments.put(variable, textureIdOrVariable);
	}
	
	public void setParentModelId(String modelId)
	{
		m_parentId = modelId;
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
			"\tparent: " + m_parentId + "\n" +
			"\telements: " + m_elements + "\n" + 
			"\ttexture assignments: " + m_textureAssignments;
	}
	
	private static boolean isVariable(String data)
	{
		return data != null && data.startsWith("#");
	}
}
