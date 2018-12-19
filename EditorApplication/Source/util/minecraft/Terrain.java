package util.minecraft;

import java.io.FileNotFoundException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.PriorityQueue;
import java.util.Queue;

import jsdl.CuboidGeometryCreator;
import jsdl.MatteOpaqueMaterialCreator;
import jsdl.ModelActorCreator;
import jsdl.SDLCommand;
import jsdl.SDLGeometry;
import jsdl.SDLMaterial;
import jsdl.SDLVector3;
import util.Vector3f;
import util.Vector3i;

public class Terrain
{
	private Map<RegionCoord, RegionData> m_regions;
	
	public Terrain()
	{
		m_regions = new HashMap<>();
	}
	
	public RegionData getRegion(int regionX, int regionZ)
	{
		return m_regions.get(new RegionCoord(regionX, regionZ));
	}
	
	public void addRegion(RegionData region)
	{
		m_regions.put(region.getRegionCoord(), region);
	}
	
	public List<SectionUnit> getAllSections()
	{
		List<SectionUnit> sections = new ArrayList<>();
		for(RegionData region : m_regions.values())
		{
			for(int chunkZ = 0; chunkZ < RegionData.NUM_CHUNKS_Z; ++chunkZ)
			{
				for(int chunkX = 0; chunkX < RegionData.NUM_CHUNKS_X; ++chunkX)
				{
					ChunkData chunk = region.getChunk(chunkX, chunkZ);
					if(chunk == null)
					{
						continue;
					}
					
					for(int s = 0; s < ChunkData.NUM_SECTIONS; ++s)
					{
						SectionData section = chunk.getSection(s);
						if(section == null)
						{
							continue;
						}
						
						final int x = region.getX() + chunkX * ChunkData.SIZE_X;
						final int y = region.getY() + s * ChunkData.SIZE_Y;
						final int z = region.getZ() + chunkZ * ChunkData.SIZE_Z;
						sections.add(new SectionUnit(new Vector3i(x, y, z), section));
					}
				}
			}
		}
		return sections;
	}
	
	public List<SectionUnit> getReachableSections(Vector3f viewpoint)
	{
		class Chamber implements Comparable<Chamber>
		{
			Vector3i coord     = null;
			EFacing  floodFrom = null;
			
			Chamber(Vector3i coord, EFacing floodFrom)
			{
				this.coord     = coord;
				this.floodFrom = floodFrom;
			}
			
			@Override
			public int compareTo(Chamber other)
			{
				Vector3f p1 = coord.toVector3f();
				Vector3f p2 = other.coord.toVector3f();
				float value1 = viewpoint.sub(p1).squareLength();
				float value2 = viewpoint.sub(p2).squareLength();
				return value1 < value2 ? -1 : (value1 > value2 ? 1 : 0);
			}
		}
		
		Vector3i[] coordOffsets = new Vector3i[]{
			new Vector3i( 0,  0, -1),
			new Vector3i( 0,  0,  1),
			new Vector3i(-1,  0,  0),
			new Vector3i( 1,  0,  0),
			new Vector3i( 0, -1,  0),
			new Vector3i( 0,  1,  0)
		};
		
		SectionStateMap floodedChamber = new SectionStateMap();
		Map<Vector3i, SectionUnit> sectionReachability = new HashMap<>();
		List<SectionUnit> reachableSections = new ArrayList<>();
		
		Map<Vector3i, SectionUnit> sectionMap = new HashMap<>();
		for(SectionUnit section : getAllSections())
		{
			sectionMap.put(section.getCoord(), section);
		}
		
		PriorityQueue<Chamber> chamberQueue = new PriorityQueue<>();
//		chamberQueue.add(toSectionCoord(viewpoint));
//		while(!coordQueue.isEmpty())
//		{
//			Vector3i currentCoord = coordQueue.poll();
//			Chamber currentVertex = vertexMap.get(currentCoord);
//			
//			Vector3i currentCoord = vertexQueue.poll();
//			Vertex currentVertex = vertexMap.get(currentCoord);
//			if(currentVertex != null)
//			{
//				currentVertex.isVisited = true;
//				
//				if(currentVertex.isSpace())
//				{
//					for(int facing = 0; facing < EFacing.SIZE; ++facing)
//					{
//						Vector3i nextCoord = currentCoord.add(coordOffsets[facing]);
//						Vertex nextVertex = vertexMap.get(nextCoord);
//						
//						// unexplored space found
//						if(nextVertex == null)
//						{
//							nextVertex = new Vertex();
//							vertexMap.put(nextCoord, nextVertex);
//							vertexQueue.add(nextCoord);
//						}
//						else if(!nextVertex.isVisited)
//						{
//							if(!nextVertex.isSpace() && !nextVertex.isAdded())
//							{
//								SectionData nextData = nextVertex.section.getData();
//								currentVertex.reachability = nextData.determinReachability();
//								reachableSections.add(nextVertex.section);
//							}
//							
//							vertexQueue.add(nextCoord);
//						}
//					}
//				}
//				else
//				{
//					SectionData sectionData = currentVertex.section.getData();
//				}
//				
//				
//				
//			}
//			else
//			{
//				
//			}
			
//			if(sectionMap.isEmpty())
//			{
//				break;
//			}
//		}
		
		
		return null;
	}
	
	private static Vector3i toSectionCoord(Vector3f viewpoint)
	{
		float clampedY = Math.max(0.0f, Math.min(viewpoint.y, ChunkData.SIZE_Y));
		
		return new Vector3i(
			Math.floorDiv((int)Math.floor(viewpoint.x), RegionData.NUM_CHUNKS_X),
			Math.max((int)clampedY / ChunkData.NUM_SECTIONS, ChunkData.NUM_SECTIONS - 1),
			Math.floorDiv((int)Math.floor(viewpoint.z), RegionData.NUM_CHUNKS_Z));
	}
	
	// FIXME: this is wrong, use floor
//	private static RegionCoord toRegionCoord(Vector3f viewpoint)
//	{
//		int regionX = (int)viewpoint.x / RegionData.SIZE_X;
//		int regionZ = (int)viewpoint.z / RegionData.SIZE_Z;
//		
//		return new RegionCoord(regionX, regionZ);
//	}
}
