package minecraft.parser;

import java.io.InputStream;

import minecraft.CuboidElement;
import minecraft.EFacing;
import minecraft.Face;
import minecraft.JSONArray;
import minecraft.JSONObject;
import minecraft.ModelData;
import util.Vector3f;

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
		ModelData model = new ModelData();
		
		if(root.has("parent"))
		{
			model.setParentModel(root.getString("parent"));
		}
		
		if(root.has("elements"))
		{
			JSONArray elements = root.getArray("elements");
			
			for(int i = 0; i < elements.numValues(); ++i)
			{
				JSONObject element = elements.getObject(i);
				JSONArray from = element.getArray("from");
				JSONArray to = element.getArray("to");
				
				CuboidElement cuboid = new CuboidElement(
					new Vector3f((float)from.getNumber(0), (float)from.getNumber(1), (float)from.getNumber(2)),
					new Vector3f((float)to.getNumber(0), (float)to.getNumber(1), (float)to.getNumber(2)));
				
				if(element.has("faces"))
				{
					JSONObject faces = element.getChild("faces");
					
					if(faces.has("down"))
					{
						parseFace(faces.getChild("down"), EFacing.DOWN, cuboid);
					}
					
					if(faces.has("up"))
					{
						parseFace(faces.getChild("up"), EFacing.UP, cuboid);
					}
					
					if(faces.has("north"))
					{
						parseFace(faces.getChild("north"), EFacing.NORTH, cuboid);
					}
					
					if(faces.has("south"))
					{
						parseFace(faces.getChild("south"), EFacing.SOUTH, cuboid);
					}
					
					if(faces.has("west"))
					{
						parseFace(faces.getChild("west"), EFacing.WEST, cuboid);
					}
					
					if(faces.has("east"))
					{
						parseFace(faces.getChild("east"), EFacing.EAST, cuboid);
					}
				}
				
				model.addElement(cuboid);
			}
		}// end elements
		
		if(root.has("textures"))
		{
			JSONObject textures = root.getChild("textures");
			for(String variable : textures.getNames())
			{
				if(variable == "particle")
				{
					continue;
				}
				
				// note that variable does not start with '#' here, we need 
				// to add it manually
				model.assignTexture("#" + variable, textures.getString(variable));
			}
		}
		
		return model;
	}
	
	private static void parseFace(JSONObject face, EFacing facing, CuboidElement out_cuboid)
	{
		Face cuboidFace = new Face();
		
		if(face.has("uv"))
		{
			JSONArray uv = face.getArray("uv");
			cuboidFace.setUVMin((float)uv.getNumber(0), (float)uv.getNumber(1));
			cuboidFace.setUVMax((float)uv.getNumber(2), (float)uv.getNumber(3));
		}
		
		String textureVariable = face.getString("texture");
		cuboidFace.setTextureVariable(textureVariable);
		
		out_cuboid.setFace(facing, cuboidFace);
	}
}
