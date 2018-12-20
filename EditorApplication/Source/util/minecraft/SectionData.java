package util.minecraft;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class SectionData
{
	public static final int SIZE_X = 16;
	public static final int SIZE_Y = 16;
	public static final int SIZE_Z = 16;
	
	private static Set<String> blockingBlocks = new HashSet<>();
	
	private List<String>              m_blockIdNames;
	private List<Map<String, String>> m_stateProperties;
	private short[][][]               m_blockIndices;
	
	public SectionData()
	{
		m_blockIdNames    = new ArrayList<>();
		m_stateProperties = new ArrayList<>();
		m_blockIndices    = new short[SIZE_Y][SIZE_Z][SIZE_X];
	}
	
	public int numBlockTypes()
	{
		return m_blockIdNames.size();
	}
	
	public String getBlockIdName(int index)
	{
		return m_blockIdNames.get(index);
	}
	
	public String getBlockIdName(int x, int y, int z)
	{
		int referencedBlockIndex = m_blockIndices[y][z][x];
		return getBlockIdName(referencedBlockIndex);
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
	
	/**
	 * Determines visibility between a section's 6 faces. The algorithm is
	 * based on Tommo's blog post, and slightly modified for ray tracing in
	 * minecraft.
	 * 
	 * See <a href="https://tomcc.github.io/2014/08/31/visibility-1.html">https://tomcc.github.io/2014/08/31/visibility-1.html</a>
	 */
	public FaceReachability determinReachability()
	{
		FaceReachability reachability = new FaceReachability();
		boolean[][][]    floodTable   = makeFloodTable();
		for(int y = 0; y < SIZE_Y; ++y)
		{
			for(int z = 0; z < SIZE_Z; ++z)
			{
				for(int x = 0; x < SIZE_X; ++x)
				{
					boolean[] reachedFaces = new boolean[EFacing.SIZE];
					floodFromBlockDFS(x, y, z, floodTable, reachedFaces);
					
					for(int a = 0; a < EFacing.SIZE - 1; ++a)
					{
						if(!reachedFaces[a])
						{
							continue;
						}
						
						for(int b = a + 1; b < EFacing.SIZE; ++b)
						{
							if(!reachedFaces[b])
							{
								continue;
							}
							
							reachability.setReachability(
								EFacing.fromValue(a), 
								EFacing.fromValue(b), 
								true);
						}
					}
					
					if(reachability.isFullyReachable())
					{
						return reachability;
					}
				}
			}
		}
		
		return reachability;
	}
	
	public static void addBlockingBlock(String blockIdName)
	{
		blockingBlocks.add(blockIdName);
	}
	
	private boolean[][][] makeFloodTable()
	{
		boolean[][][] floodTable = new boolean[SIZE_Y][SIZE_Z][SIZE_X];
		for(int y = 0; y < SIZE_Y; ++y)
		{
			for(int z = 0; z < SIZE_Z; ++z)
			{
				for(int x = 0; x < SIZE_X; ++x)
				{
					if(blockingBlocks.contains(getBlockIdName(x, y, z)))
					{
						floodTable[y][z][x] = true;
					}
				}
			}
		}
		return floodTable;
	}
	
	private static void floodFromBlockDFS(int x, int y, int z, boolean[][][] isFlooded, boolean[] reachedFaces)
	{
		if(isFlooded[y][z][x])
		{
			return;
		}
		
		isFlooded[y][z][x] = true;
		
		if(x == 0)
		{
			reachedFaces[EFacing.WEST.getValue()] = true;
			floodFromBlockDFS(x + 1, y, z, isFlooded, reachedFaces);
		}
		else if(x + 1 < SIZE_X)
		{
			floodFromBlockDFS(x - 1, y, z, isFlooded, reachedFaces);
			floodFromBlockDFS(x + 1, y, z, isFlooded, reachedFaces);
		}
		else
		{
			reachedFaces[EFacing.EAST.getValue()] = true;
			floodFromBlockDFS(x - 1, y, z, isFlooded, reachedFaces);
		}
		
		if(z == 0)
		{
			reachedFaces[EFacing.NORTH.getValue()] = true;
			floodFromBlockDFS(x, y, z + 1, isFlooded, reachedFaces);
		}
		else if(z + 1 < SIZE_Z)
		{
			floodFromBlockDFS(x, y, z - 1, isFlooded, reachedFaces);
			floodFromBlockDFS(x, y, z + 1, isFlooded, reachedFaces);
		}
		else
		{
			reachedFaces[EFacing.SOUTH.getValue()] = true;
			floodFromBlockDFS(x, y, z - 1, isFlooded, reachedFaces);
		}
		
		if(y == 0)
		{
			reachedFaces[EFacing.DOWN.getValue()] = true;
			floodFromBlockDFS(x, y + 1, z, isFlooded, reachedFaces);
		}
		else if(y + 1 < SIZE_Y)
		{
			floodFromBlockDFS(x, y - 1, z, isFlooded, reachedFaces);
			floodFromBlockDFS(x, y + 1, z, isFlooded, reachedFaces);
		}
		else
		{
			reachedFaces[EFacing.UP.getValue()] = true;
			floodFromBlockDFS(x, y - 1, z, isFlooded, reachedFaces);
		}
	}
	
	@Override
	public String toString()
	{
		return "Section: " + m_blockIdNames;
	}
}
