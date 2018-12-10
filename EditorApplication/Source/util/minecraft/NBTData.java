package util.minecraft;

import java.util.ArrayList;
import java.util.HashMap;

public class NBTData
{
	private HashMap<String, Object> m_namedData;
	
	public NBTData()
	{
		m_namedData = new HashMap<>();
	}
	
	public void addByte(String name, byte data)
	{
		addData(name, data);
	}
	
	public void addShort(String name, short data)
	{
		addData(name, data);
	}
	
	public void addInt(String name, int data)
	{
		addData(name, data);
	}
	
	public void addLong(String name, long data)
	{
		addData(name, data);
	}
	
	public void addFloat(String name, float data)
	{
		addData(name, data);
	}
	
	public void addDouble(String name, double data)
	{
		addData(name, data);
	}
	
	public void addByteArray(String name, byte[] data)
	{
		addData(name, data);
	}
	
	public void addString(String name, String data)
	{
		addData(name, data);
	}
	
	public void addList(String name, ArrayList<?> data)
	{
		addData(name, data);
	}
	
	public void addCompound(String name, NBTData data)
	{
		addData(name, data);
	}
	
	public void addIntArray(String name, int[] data)
	{
		addData(name, data);
	}
	
	public void addLongArray(String name, long[] data)
	{
		addData(name, data);
	}
	
	private void addData(String name, Object data)
	{
		if(m_namedData.containsKey(name))
		{
			Object existingData = m_namedData.get(name);
			String existingType = existingData.getClass().getName();
			String newType      = data.getClass().getName();
			
			System.err.println(
				"warning: data name <" + name + "> exists, replacing " + 
				"existing type <" + existingType + "> with <" + newType + ">");
		}
		
		m_namedData.put(name, data);
	}
}
