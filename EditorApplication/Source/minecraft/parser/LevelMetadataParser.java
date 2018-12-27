package minecraft.parser;

import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.List;
import java.util.zip.GZIPInputStream;

import minecraft.LevelMetadata;
import minecraft.nbt.NBTData;
import util.Vector3f;

public class LevelMetadataParser
{
	public LevelMetadataParser()
	{}
	
	public LevelMetadata parse(Path levelFolder)
	{
		LevelMetadata levelMetadata = new LevelMetadata();
		
		Path levelDatFile = levelFolder.resolve("level.dat");
		try(InputStream rawData = new GZIPInputStream(Files.newInputStream(levelDatFile)))
		{
			NBTData root = new NBTParser().parse(rawData).get("");
			NBTData data = root.get("Data");
			NBTData player = data.get("Player");
			
			List<Double> pos = player.get("Pos");
			List<Float> rotation = player.get("Rotation");
			levelMetadata.setSpPlayerPosition(new Vector3f(pos.get(0).floatValue(), pos.get(1).floatValue(), pos.get(2).floatValue()));
			levelMetadata.setSpPlayerYawDegrees(rotation.get(0));
			levelMetadata.setSpPlayerPitchDegrees(rotation.get(1));
		}
		catch(IOException e)
		{
			e.printStackTrace();
		}
		
		return levelMetadata;
	}
}
