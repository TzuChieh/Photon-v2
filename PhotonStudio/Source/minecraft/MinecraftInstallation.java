package minecraft;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;
import java.util.stream.Stream;

public class MinecraftInstallation
{
	private Path m_home;
	
	public MinecraftInstallation()
	{
		String osName = System.getProperty("os.name").toLowerCase();
		if(osName.contains("win"))
		{
			m_home = Paths.get(System.getenv("APPDATA"), "\\.minecraft");
		}
		else if(osName.contains("nux"))
		{
			m_home = Paths.get("~/.minecraft");
		}
		else if(osName.contains("mac"))
		{
			m_home = Paths.get("~/Library/Application Support/minecraft");
		}
		else
		{
			System.err.println("warning: unidentified OS <" + osName + ">, assuming windows");
			
			// FIXME: duplicated code
			m_home = Paths.get(System.getenv("APPDATA"), "\\.minecraft");
		}
		
		if(!Files.isDirectory(m_home))
		{
			System.err.println("warning: home directory not found");
			m_home = null;
		}
	}
	
	public Path getHome()
	{
		return m_home;
	}
	
	public Path getJar(int majorVersion, int minorVersion)
	{
		if(m_home == null)
		{
			return null;
		}
		
		String fullVersion = "1." + majorVersion + "." + minorVersion;
		Path jarPath = m_home.resolve("versions").resolve(fullVersion).resolve(fullVersion + ".jar");
		if(!Files.isRegularFile(jarPath))
		{
			return null; 
		}
		
		return jarPath;
	}
	
	public List<Path> getLevels()
	{
		List<Path> levels = new ArrayList<>();
		
		if(m_home == null)
		{
			return levels;
		}
		
		try(Stream<Path> paths = Files.list(m_home.resolve("saves")))
		{
		    paths.filter(Files::isDirectory).forEach((Path level) -> 
		    {
		    	levels.add(level);
		    });
		}
		catch(IOException e)
		{
			MCLogger.warn("error during level finding");
			e.printStackTrace();
		}
		
		return levels;
	}
}
