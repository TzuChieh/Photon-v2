package util.minecraft;

import java.util.HashMap;

public class NBTData
{
	private HashMap<String, NBTTag> m_tags;
	
	public NBTData()
	{
		m_tags = new HashMap<>();
	}
	
	@SuppressWarnings("unchecked")
	public <T extends NBTTag> T get(String name)
	{
		if(!m_tags.containsKey(name))
		{
			System.err.println("warning: data <" + name + "> does not exist");
		}
		
		return (T)m_tags.get(name);
	}
	
	public <T extends NBTTag> void add(T tag)
	{
		if(m_tags.containsKey(tag.getName()))
		{
			NBTTag existingTag  = m_tags.get(tag.getName());
			String existingType = existingTag.getClass().getName();
			String newType      = tag.getClass().getName();
			
			System.err.println(
				"warning: data <" + tag.getName() + "> exists, replacing " + 
				"existing type <" + existingType + "> with <" + newType + ">");
		}
		
		m_tags.put(tag.getName(), tag);
	}
}
