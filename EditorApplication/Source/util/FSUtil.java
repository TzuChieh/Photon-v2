package util;

import java.io.File;
import java.net.URISyntaxException;
import java.net.URL;

public final class FSUtil
{
	public static String toSeparatorEnded(String path)
	{
		if(path.charAt(path.length() - 1) != File.separatorChar)
		{
			return path + File.separatorChar;
		}
		
		return path; 
	}
	
	public static String getResourcePath()
	{
		URL location = FSUtil.class.getProtectionDomain().getCodeSource().getLocation();
		
		try
		{
			return toSeparatorEnded(new File(location.toURI()).getPath().toString());
		}
		catch(URISyntaxException e)
		{
			e.printStackTrace();
		}
		
		return "";
	}
}
