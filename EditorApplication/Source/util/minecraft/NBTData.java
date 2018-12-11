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
	
	public byte getByte(String name)
	{
		return (byte)getData(name);
	}
	
	public short getShort(String name)
	{
		return (short)getData(name);
	}
	
	public int getInt(String name)
	{
		return (int)getData(name);
	}
	
	public long getLong(String name)
	{
		return (long)getData(name);
	}
	
	public float getFloat(String name)
	{
		return (float)getData(name);
	}
	
	public double getDouble(String name)
	{
		return (double)getData(name);
	}
	
	public byte[] getByteArray(String name)
	{
		return (byte[])getData(name);
	}
	
	public String getString(String name)
	{
		return (String)getData(name);
	}
	
	public NBTList getList(String name)
	{
		return (NBTList)getData(name);
	}
	
	public NBTData getCompound(String name)
	{
		return (NBTData)getData(name);
	}
	
	public int[] getIntArray(String name)
	{
		return (int[])getData(name);
	}
	
	public long[] getLongArray(String name)
	{
		return (long[])getData(name);
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
	
	private Object getData(String name)
	{
		if(!m_namedData.containsKey(name))
		{
			System.err.println("warning: data name <" + name + "> not exist");
		}
		
		return m_namedData.get(name);
	}
}
