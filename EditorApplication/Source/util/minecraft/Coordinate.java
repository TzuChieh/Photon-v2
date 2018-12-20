package util.minecraft;

import util.Vector3i;

public class Coordinate
{
	public static Vector3i toSection(Vector3i block)
	{
		return new Vector3i(
			Math.floorDiv(block.x, SectionData.SIZE_X),
			Math.floorDiv(block.y, SectionData.SIZE_Y),
			Math.floorDiv(block.z, SectionData.SIZE_Z));
	}
	
	public static Vector3i toRegion(Vector3i block)
	{
		return new Vector3i(
			Math.floorDiv(block.x, RegionData.SIZE_X),
			Math.floorDiv(block.y, RegionData.SIZE_Y),
			Math.floorDiv(block.z, RegionData.SIZE_Z));
	}
	
	public static RegionCoord sectionToRegion(Vector3i section)
	{
		return new RegionCoord(
			Math.floorDiv(section.x, RegionData.NUM_CHUNKS_X),
			Math.floorDiv(section.z, RegionData.NUM_CHUNKS_Z));
	}
}
