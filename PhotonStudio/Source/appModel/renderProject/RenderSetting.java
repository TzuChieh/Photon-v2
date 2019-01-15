package appModel.renderProject;

import appModel.GeneralOption;
import appModel.Setting;
import appModel.SettingGroup;

public class RenderSetting extends SettingGroup
{
	public static final String SCENE_FILE_PATH = "scene-file-path";
	public static final String NUM_THREADS     = "num-threads";
	
	private GeneralOption m_generalOption;
	
	public RenderSetting(GeneralOption generalOption)
	{
		super();
		
		m_generalOption = generalOption;
		
		add(SCENE_FILE_PATH, m_generalOption.get(GeneralOption.DEFAULT_SCENE_FILE_PATH));
		add(NUM_THREADS,     "4");
	}
	
	public Setting getSceneFilePath()
	{
		return getSetting(SCENE_FILE_PATH);
	}
	
	public Setting getNumThreads()
	{
		return getSetting(NUM_THREADS);
	}
}
