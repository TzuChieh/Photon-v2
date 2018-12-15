package util.minecraft;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

public class ChunkSection
{
	public static final int SIZE_X = 16;
	public static final int SIZE_Y = 16;
	public static final int SIZE_Z = 16;
	
	private List<String>              m_blockIdNames;
	private List<Map<String, String>> m_stateProperties;
	private short[][][]               m_blockIndices;
	
	public ChunkSection()
	{
		m_blockIdNames    = new ArrayList<>();
		m_stateProperties = new ArrayList<>();
		m_blockIndices    = new short[SIZE_Y][SIZE_Z][SIZE_X];
	}
	
	public void addBlock(String blockIdName, Map<String, String> stateProperties)
	{
		m_blockIdNames.add(blockIdName);
		m_stateProperties.add(stateProperties);
	}
	
	public void setLayer(int y, short[][] indices)
	{
		for(int z = 0; z < SIZE_Z; ++z)
		{
			for(int x = 0; x < SIZE_X; ++x)
			{
				m_blockIndices[y][z][x] = indices[z][x];
			}
		}
	}
}
