package minecraft;

import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

public class MinecraftInstallation
{
	private Path m_homePath;
	
	public MinecraftInstallation()
	{
		String osName = System.getProperty("os.name").toLowerCase();
		if(osName.contains("win"))
		{
			m_homePath = Paths.get(System.getenv("APPDATA"), "\\.minecraft");
		}
		else if(osName.contains("nux"))
		{
			m_homePath = Paths.get("~/.minecraft");
		}
		else if(osName.contains("mac"))
		{
			m_homePath = Paths.get("~/Library/Application Support/minecraft");
		}
		else
		{
			System.err.println("warning: unidentified OS <" + osName + ">, assuming windows");
			
			// FIXME: duplicated code
			m_homePath = Paths.get(System.getenv("APPDATA"), "\\.minecraft");
		}
		
		if(!Files.isDirectory(m_homePath))
		{
			System.err.println("warning: home directory not found");
			m_homePath = null;
		}
	}
	
	public Path getHomePath()
	{
		return m_homePath;
	}
	
	public Path getJarPath(int majorVersion, int minorVersion)
	{
		if(m_homePath == null)
		{
			return null;
		}
		
		String fullVersion = "1." + majorVersion + "." + minorVersion;
		Path jarPath = m_homePath.resolve("versions/" + fullVersion + "/" + fullVersion + ".jar");
		if(!Files.isRegularFile(jarPath))
		{
			return null; 
		}
		
		return jarPath;
	}
}
