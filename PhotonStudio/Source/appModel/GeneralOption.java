package appModel;

import java.io.File;
import java.nio.file.Paths;

import util.FSUtil;

public final class GeneralOption extends SettingGroup
{
	public static final String WORKING_DIRECTORY       = "working-directory";
	public static final String DEFAULT_SCENE_FILE_PATH = "default-scene-file-path";
	
	public GeneralOption()
	{
		super();
	}
	
	@Override
	public void setToDefaults()
	{
		set(WORKING_DIRECTORY,       getDefaultAbsWorkingDirectory());
		set(DEFAULT_SCENE_FILE_PATH, getDefaultSceneFilePath());
	}
	
	public void save()
	{
		saveToFile(getStoredP2CfgAbsPath());
	}
	
	public void load()
	{
		File file = new File(getStoredP2CfgAbsPath());
		if(file.exists())
		{
			loadFromFile(getStoredP2CfgAbsPath());
		}
		else
		{
			setToDefaults();
		}
	}
	
	private String getDefaultAbsWorkingDirectory()
	{
		String directory = Paths.get("./").toAbsolutePath().normalize().toString();
		
		return FSUtil.toSeparatorEnded(directory);
	}
	
	private String getDefaultSceneFilePath()
	{
		return FSUtil.getResourceDirectory() + "scenes/default_scene.p2";
	}
	
	private String getStoredP2CfgAbsPath()
	{
		return getDefaultAbsWorkingDirectory() + "general-options.p2cfg";
	}
}
