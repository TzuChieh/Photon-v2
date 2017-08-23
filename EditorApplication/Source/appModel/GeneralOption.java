package appModel;

import java.io.File;
import java.net.URISyntaxException;
import java.net.URL;
import java.nio.file.Paths;

public final class GeneralOption extends SettingGroup
{
	public static final String WORKING_DIRECTORY      = "working-directory";
	public static final String DEFAULT_SCENE_ABS_PATH = "default-scene-abs-path";
	
	public GeneralOption()
	{
		super();
	}
	
	@Override
	public void setToDefaults()
	{
		set(WORKING_DIRECTORY,      getCurrentAbsWorkingDirectory());
		set(DEFAULT_SCENE_ABS_PATH, getDefaultSceneAbsPath());
	}
	
	private String getCurrentAbsWorkingDirectory()
	{
		return Paths.get(".").toAbsolutePath().normalize().toString();
	}
	
	private String getDefaultSceneAbsPath()
	{
		String defaultScenePath = "/internalRes/default_scene.p2";
		URL    defaultSceneUrl  = this.getClass().getResource(defaultScenePath);
		try
		{
			File file = new File(defaultSceneUrl.toURI());
			defaultScenePath = file.getCanonicalPath();
			
			// this code should be okay, but gives a leading "/" which is weird 
//			defaultScenePath = defaultSceneUrl.getPath();
		}
		catch(Exception e)
		{
			e.printStackTrace();
			System.err.println("couldn't locate default scene file");
			
			defaultScenePath = "";
		}
		
		return defaultScenePath;
	}
}
