package util.minecraft;

public class NBTList
{
	private Object m_listData;
	
	public NBTList()
	{
		this(null);
	}
	
	public NBTList(Object listData)
	{
		m_listData = listData;
	}
	
	public byte[] getBytes()
	{
		return (byte[])m_listData;
	}
	
	public short[] getShorts()
	{
		return (short[])m_listData;
	}
	
	public int[] getInts()
	{
		return (int[])m_listData;
	}
	
	public long[] getLongs()
	{
		return (long[])m_listData;
	}
	
	public float[] getFloats()
	{
		return (float[])m_listData;
	}
	
	public double[] getDoubles()
	{
		return (double[])m_listData;
	}
	
	public byte[][] getByteArrays()
	{
		return (byte[][])m_listData;
	}
	
	public String[] getStrings()
	{
		return (String[])m_listData;
	}
	
	public NBTList[] getLists()
	{
		return (NBTList[])m_listData;
	}
	
	public NBTData[] getCompounds()
	{
		return (NBTData[])m_listData;
	}
	
	public int[][] getIntArrays()
	{
		return (int[][])m_listData;
	}
	
	public long[][] getLongArrays()
	{
		return (long[][])m_listData;
	}
	
	public void set(Object listData)
	{
		m_listData = listData;
	}
}
