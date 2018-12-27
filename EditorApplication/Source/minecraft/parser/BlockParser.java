package minecraft.parser;

import java.io.InputStream;

import minecraft.JSONArray;
import minecraft.JSONObject;
import minecraft.block.BlockData;
import minecraft.block.BlockModel;
import minecraft.block.BlockVariant;

public class BlockParser
{
	private JSONParser m_jsonParser;
	
	public BlockParser()
	{
		m_jsonParser = new JSONParser();
	}
	
	public BlockData parse(InputStream rawData)
	{
		BlockData block = new BlockData();
		JSONObject root = m_jsonParser.parse(rawData);
		
		if(root.has("variants"))
		{
			JSONObject variants = root.getChild("variants");
			
			// (variant name consists of the relevant block states separated by commas)
			for(String variantName : variants.getNames())
			{
				BlockVariant blockVariant = new BlockVariant();
				
				if(variants.get(variantName) instanceof JSONObject)
				{
					JSONObject model = variants.getChild(variantName);
					blockVariant.addModel(parseBlockModel(model));
				}
				else
				{
					JSONArray models = variants.getArray(variantName);
					for(int i = 0; i < models.numValues(); ++i)
					{
						JSONObject model = models.getObject(i);
						blockVariant.addModel(parseBlockModel(model));
					}
				}
				
				block.addVariant(variantName, blockVariant);
			}
		}
		
		return block;
	}
	
	private static BlockModel parseBlockModel(JSONObject model)
	{
		BlockModel blockModel = new BlockModel(model.getString("model"));
		
		int xRotDegrees = 0;
		int yRotDegrees = 0;
		if(model.has("x"))
		{
			xRotDegrees = (int)model.getNumber("x");
		}
		if(model.has("y"))
		{
			yRotDegrees = (int)model.getNumber("y");
		}
		blockModel.setRotationDegrees(xRotDegrees, yRotDegrees);
		
		return blockModel;
	}
}
