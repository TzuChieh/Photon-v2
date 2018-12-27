package minecraft.parser;

import java.io.InputStream;

import minecraft.BlockData;
import minecraft.JSONObject;

public class BlockParser
{
	private JSONParser m_jsonParser;
	
	public BlockParser()
	{
		m_jsonParser = new JSONParser();
	}
	
	public BlockData parse(InputStream rawData)
	{
		JSONObject root = m_jsonParser.parse(rawData);
		
		BlockData block = new BlockData();
		
		
		return block;
	}
}
