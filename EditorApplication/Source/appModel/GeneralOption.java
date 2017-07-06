package appModel;

import java.io.File;
import java.net.URISyntaxException;
import java.net.URL;

public final class GeneralOption extends SettingGroup
{
	public static final int DEFAULT_SCENE_PATH = 10;
	
	public GeneralOption()
	{
		super();
	}
	
	@Override
	public void setToDefaults()
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
		}
		
		set(DEFAULT_SCENE_PATH, defaultScenePath);
	}
}
