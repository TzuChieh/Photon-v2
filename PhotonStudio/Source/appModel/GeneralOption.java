package appModel;

import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

import util.FSUtil;

public final class GeneralOption extends SettingGroup
{
	public static final String WORKING_DIRECTORY       = "working-directory";
	public static final String DEFAULT_SCENE_FILE_PATH = "default-scene-file-path";
	
	public GeneralOption()
	{
		super();
		
		add(WORKING_DIRECTORY,       getDefaultAbsWorkingDirectory());
		add(DEFAULT_SCENE_FILE_PATH, getDefaultSceneFilePath());
	}
	
	public void save()
	{
		save(Paths.get(getStoredP2CfgAbsPath()));
	}
	
	public void load()
	{
		Path path = Paths.get(getStoredP2CfgAbsPath());
		if(Files.exists(path))
		{
			load(path);
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
