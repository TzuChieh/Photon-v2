package minecraft.parser;

import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;
import java.util.stream.Stream;
import java.util.zip.GZIPInputStream;

import minecraft.LevelData;
import minecraft.LevelMetadata;
import minecraft.MCLogger;
import minecraft.RegionData;
import minecraft.nbt.NBTData;
import util.Vector3f;

public class LevelParser
{
	public LevelParser()
	{}
	
	public LevelData parse(Path levelFolder)
	{
		LevelData levelData = new LevelData();
		
		Path mcaFolder = levelFolder.resolve("region");
		for(RegionData region : parseMCAFiles(mcaFolder))
		{
			levelData.addRegion(region);
		}
		
		levelData.setMetadata(parseMetadata(levelFolder));
		
		return levelData;
	}
	
	private List<RegionData> parseMCAFiles(Path mcaFolder)
	{
		MCAParser        parser  = new MCAParser();
		List<RegionData> regions = new ArrayList<>();
		
		try(Stream<Path> paths = Files.list(mcaFolder))
		{
		    paths.filter(Files::isRegularFile).forEach((Path mcaFile) -> 
		    {
		    	MCLogger.log("parsing MCA file <" + mcaFile + ">");
		    	
		    	RegionData region = parser.parse(mcaFile);
		    	if(region != null)
		    	{
		    		regions.add(parser.parse(mcaFile));
		    	}
		    	else
		    	{
		    		MCLogger.warn("MCA file <" + mcaFile + "> parsing failed");
		    	}
		    });
		}
		catch(IOException e)
		{
			MCLogger.warn("error during region loading, data may be incomplete");
			e.printStackTrace();
		}
		
		return regions;
	}
	
	private LevelMetadata parseMetadata(Path levelFolder)
	{
		LevelMetadata metadata = new LevelMetadata();
		
		Path levelDatFile = levelFolder.resolve("level.dat");
		try(InputStream rawData = new GZIPInputStream(Files.newInputStream(levelDatFile)))
		{
			NBTData root = new NBTParser().parse(rawData).get("");
			NBTData data = root.get("Data");
			NBTData player = data.get("Player");
			
			List<Double> pos = player.get("Pos");
			List<Float> rotation = player.get("Rotation");
			metadata.setSpPlayerPosition(new Vector3f(pos.get(0).floatValue(), pos.get(1).floatValue(), pos.get(2).floatValue()));
			metadata.setSpPlayerYawDegrees(rotation.get(0));
			metadata.setSpPlayerPitchDegrees(rotation.get(1));
		}
		catch(IOException e)
		{
			e.printStackTrace();
		}
		
		return metadata;
	}
}
