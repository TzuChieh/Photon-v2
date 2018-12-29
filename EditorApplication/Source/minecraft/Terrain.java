package minecraft;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;

import minecraft.block.BlockStateProvider;
import util.Vector3i;

public class Terrain implements Iterable<SectionUnit>
{
	private List<SectionUnit> m_sections;
	
	public Terrain()
	{
		this(new ArrayList<>());
	}
	
	public Terrain(List<SectionUnit> sections)
	{
		m_sections = sections;
	}
	
	public void addSection(SectionUnit section)
	{
		m_sections.add(section);
	}
	
	public void addSections(List<SectionUnit> sections)
	{
		m_sections.addAll(sections);
	}
	
	public int numSections()
	{
		return m_sections.size();
	}
	
	public Set<String> getRequiredBlocks()
	{
		Set<String> blockIds = new HashSet<>();
		for(SectionUnit sectionUnit : m_sections)
		{
			SectionData section = sectionUnit.getData();
			for(int i = 0; i < section.numBlockTypes(); ++i)
			{
				blockIds.add(section.getBlockIdName(i));
			}
		}
		return blockIds;
	}

	@Override
	public Iterator<SectionUnit> iterator()
	{
		return m_sections.iterator();
	}
	
	@Override
	public String toString()
	{
		return m_sections.toString();
	}
}
