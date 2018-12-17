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
//	public FaceReachability determinReachability(EFacing visible)
//	{
//		EFacing[] reachedFaces = new EFacing[EFacing.values().length];
//		reachedFaces.
//		
//		for(int z = 0; z < SIZE_Z; ++z)
//		{
//			for(int x = 0; x < SIZE_X; ++x)
//			{
//				m_blockIndices[y][z][x] = indices[z][x];
//			}
//		}
//		
//		return new FaceReachability();
//	}
	
	public void addBlockingBlock(String blockIdName)
	{
		blockingBlocks.add(blockIdName);
	}
}
