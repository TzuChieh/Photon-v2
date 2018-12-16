package util.minecraft.parser;

import java.io.InputStream;

import util.Vector3f;
import util.minecraft.CubicElement;
import util.minecraft.JSONObject;
import util.minecraft.ModelData;

public class ModelParser
{
	private JSONParser m_jsonParser;
	
	public ModelParser()
	{
		m_jsonParser = new JSONParser();
	}
	
	public ModelData parse(InputStream rawData)
	{
		JSONObject root = m_jsonParser.parse(rawData);
		if(root.has("elements"))
		{
			ModelData model = new ModelData();
			
			//CubicElement element = new CubicElement(new Vector3f(0, 0, 0), new Vector3f(16, 16, 16));
			
			return model;
		}
		else
		{
			return null;
		}
	}
}
