package util;

import java.io.File;
import java.net.URISyntaxException;
import java.net.URL;
import java.nio.file.Path;
import java.nio.file.Paths;

public final class FSUtil
{
	// returns <project-root>/build/
	public static String getBuildDirectory()
	{
		return getJarDirectory() + "../";
	}
	
	public static String getResourceDirectory()
	{
		return getBuildDirectory() + "Resource/";
	}
	
	public static String getJarDirectory()
	{
		URL location = FSUtil.class.getProtectionDomain().getCodeSource().getLocation();
		
		Path path;
		try
		{
			// URL.toString()/getPath()/getFile()/etc.. replaces spaces with %20s 
			// (as expected in a url). Here we use toURI() to keep the plain spaces.
			path = Paths.get(location.toURI()).getParent();
		}
		catch(URISyntaxException e)
		{
			e.printStackTrace();
			return toSeparatorEnded("");
		}
		
		return toSeparatorEnded(path.toAbsolutePath().toString());
	}
	
	public static String toSeparatorEnded(String path)
	{
		if(path.charAt(path.length() - 1) != File.separatorChar)
		{
			return path + File.separatorChar;
		}
		
		return path; 
	}
}
