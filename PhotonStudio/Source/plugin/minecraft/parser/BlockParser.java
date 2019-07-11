package plugin.minecraft.parser;

import java.io.InputStream;

import plugin.minecraft.JSONArray;
import plugin.minecraft.JSONObject;
import plugin.minecraft.block.AllMatchedConditional;
import plugin.minecraft.block.Block;
import plugin.minecraft.block.BlockData;
import plugin.minecraft.block.BlockModel;

public class BlockParser
{
	private JSONParser m_jsonParser;
	
	public BlockParser()
	{
		m_jsonParser = new JSONParser();
	}
	
	public BlockData parse(InputStream rawData)
	{
		BlockData blockData = new BlockData();
		JSONObject root = m_jsonParser.parse(rawData);
		
		if(root.has("variants"))
		{
			JSONObject variants = root.getChild("variants");
			for(String variantName : variants.getNames())
			{
				Block blockVariant = new Block();
				
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
				
				// (variant name consists of the relevant block states separated by commas)
				AllMatchedConditional conditional = new AllMatchedConditional();
				conditional.addTargetStates(variantName);
				
				blockData.addBlock(conditional, blockVariant);
			}
		}
		
		return blockData;
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
