package util.minecraft.parser;

import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;

import util.BinaryData;
import util.minecraft.JSONArray;
import util.minecraft.JSONObject;

public class JSONParser
{
	public JSONParser()
	{}
	
	public JSONObject parse(InputStream rawData)
	{
		JSONObject data = null;
		try
		{
			byte[] bytes = BinaryData.readAll(rawData);
			String string = new String(bytes, StandardCharsets.UTF_8);
			
			int objectStartBrace = string.indexOf('{');
			if(objectStartBrace != -1)
			{
				data = new JSONObject();
				parseJSONObject(string, objectStartBrace, data);
			}
		}
		catch(IOException e)
		{
			System.err.println("error parsing JSON, parsed data may be corrupted");
			e.printStackTrace();
		}
		
		return data;
	}
	
	private static int parseJSONObject(String data, int objectStartBrace, JSONObject out_object)
	{
		assert(data.charAt(objectStartBrace) == '{');
		
		// check if this is an empty object
		int currentIndex = findNonWhitespace(data, objectStartBrace + 1);
		if(data.charAt(currentIndex) == '}')
		{
			return currentIndex;
		}
		
		// so this object contains member, parse them
		final int dataLength = data.length();
		while(currentIndex < dataLength)
		{
			final int colonIndex = data.indexOf(':', currentIndex + 1);
			assert(colonIndex != -1);
			
			String name = data.substring(currentIndex, colonIndex);
			name = name.replace('\"', ' ').trim();
			
			// locate the start of value
			currentIndex = findNonWhitespace(data, colonIndex + 1);
			
			Object value = null;
			switch(data.charAt(currentIndex))
			{
			// start of JSON object
			case '{':
			{
				JSONObject childObject = new JSONObject();
				int childObjectEndBrace = parseJSONObject(data, currentIndex, childObject);
				value = childObject;
				currentIndex = childObjectEndBrace + 1;
			}
			break;
				
			// start of JSON array
			case '[':
			{
				JSONArray array = new JSONArray();
				int arrayEndBracket = parseJSONArray(data, currentIndex, array);
				value = array;
				currentIndex = arrayEndBracket + 1;
			}
			break;
				
			// may be the start of string, boolean, number, null
			default:
			{
				int valueEndIndex = findValueDelimiter(data, currentIndex + 1);
				String string = data.substring(currentIndex, valueEndIndex);
				string = string.replace('\"', ' ').trim();
				value = string;
				currentIndex = valueEndIndex;
			}
			break;
			}
			
			if(value != null)
			{
				out_object.add(name, value);
			}
			
			currentIndex = findValueDelimiter(data, currentIndex + 1);
			if(data.charAt(currentIndex) == '}')
			{
				break;
			}
			assert(data.charAt(currentIndex) == ',');
			++currentIndex;
		}
		
		return currentIndex;
	}
	
	private static int parseJSONArray(String data, int arrayStartBracket, JSONArray out_array)
	{
		assert(data.charAt(arrayStartBracket) == '[');
		
		// check if this is an empty array
		int currentIndex = findNonWhitespace(data, arrayStartBracket + 1);
		if(data.charAt(currentIndex) == ']')
		{
			return currentIndex;
		}
		
		// so this array contains value, parse them
		final int dataLength = data.length();
		while(currentIndex < dataLength)
		{
			Object value = null;
			switch(data.charAt(currentIndex))
			{
			// start of JSON object
			case '{':
			{
				JSONObject object = new JSONObject();
				int objectEndBrace = parseJSONObject(data, currentIndex, object);
				value = object;
				currentIndex = objectEndBrace + 1;
			}
			break;
				
			// start of JSON array
			case '[':
			{
				JSONArray childArray = new JSONArray();
				int childArrayEndBracket = parseJSONArray(data, currentIndex, childArray);
				value = childArray;
				currentIndex = childArrayEndBracket + 1;
			}
			break;
				
			// may be the start of string, boolean, number, null
			default:
			{
				int valueEndIndex = findValueDelimiter(data, currentIndex + 1);
				String string = data.substring(currentIndex, valueEndIndex);
				string = string.replace('\"', ' ').trim();
				value = string;
				currentIndex = valueEndIndex;
			}
			break;
			}
			
			if(value != null)
			{
				out_array.add(value);
			}
			
			currentIndex = findValueDelimiter(data, currentIndex + 1);
			if(data.charAt(currentIndex) == ']')
			{
				break;
			}
			assert(data.charAt(currentIndex) == ',');
			++currentIndex;
		}
		
		return currentIndex;
	}
	
	private static int findNonWhitespace(String data, int startIndex)
	{
		if(startIndex == -1)
		{
			return -1;
		}
		
		final int dataLength = data.length();
		
		int currentIndex = startIndex;
		while(currentIndex < dataLength)
		{
			char ch = data.charAt(currentIndex);
			if(!Character.isWhitespace(ch))
			{
				return currentIndex;
			}
			else
			{
				++currentIndex;
			}
		}
		
		return -1;
	}
	
	private static int findValueDelimiter(String data, int startIndex)
	{
		if(startIndex == -1)
		{
			return -1;
		}
		
		final int dataLength = data.length();
		
		int currentIndex = startIndex;
		while(currentIndex < dataLength)
		{
			char ch = data.charAt(currentIndex);
			if(ch == ',' || ch == '}' || ch == ']')
			{
				return currentIndex;
			}
			else
			{
				++currentIndex;
			}
		}
		
		return -1;
	}
}
