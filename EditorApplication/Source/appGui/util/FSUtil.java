package appGui.util;

import java.io.File;

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
}
