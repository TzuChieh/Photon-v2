package plugin.minecraft.nbt;

import java.util.Collection;
import java.util.HashMap;
import java.util.Map;

public class NBTData
{
	private HashMap<String, NBTTag> m_tags;
	
	public NBTData()
	{
		m_tags = new HashMap<>();
	}
	
	/**
	 * Gets a tag's payload.
	 * @param name Name of the tag.
	 * @return The payload of the target tag.
	 */
	public <T> T get(String name)
	{
		return getTag(name).getPayload();
	}
	
	/**
	 * Gets a NBT tag.
	 * @param name Name of the tag.
	 * @return The tag with the specified name. Returns null if tag is not found.
	 */
	@SuppressWarnings("unchecked")
	public <T extends NBTTag> T getTag(String name)
	{
		if(!m_tags.containsKey(name))
		{
			System.err.println("warning: data <" + name + "> does not exist");
		}
		
		return (T)m_tags.get(name);
	}
	
	public <T extends NBTTag> void addTag(T tag)
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
	
	public boolean hasTag(String name)
	{
		return m_tags.containsKey(name);
	}
	
	public Collection<NBTTag> getTags()
	{
		return m_tags.values();
	}
	
	@Override
	public String toString()
	{
		String result = "";
		for(Map.Entry<String, NBTTag> entry : m_tags.entrySet())
		{
			result += entry.getValue().toString() + "\n";
		}
		return result;
	}
}
