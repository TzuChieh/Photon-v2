package minecraft.parser;

import java.io.IOException;
import java.io.InputStream;

import minecraft.nbt.CompoundTag;
import minecraft.nbt.NBTData;

public class NBTParser
{
	public NBTParser()
	{}
	
	public NBTData parse(InputStream rawData)
	{
		// treating the file itself as a compound tag
		CompoundTag fileTag = new CompoundTag();
		
		try
		{
			// the content of the file is its payload
			fileTag.setPayload(rawData);
		}
		catch(IOException e)
		{
			System.err.println("error parsing NBT, parsed data may be corrupted");
			e.printStackTrace();
		}
		
		return fileTag.getPayload();
	}
}
