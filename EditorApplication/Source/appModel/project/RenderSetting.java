package appModel.project;

import appModel.GeneralOption;
import appModel.SettingGroup;

public final class RenderSetting extends SettingGroup
{
	public static final String SCENE_FILE_NAME = "scene-filename";
	
	private GeneralOption m_generalOption;
	
	public RenderSetting(GeneralOption generalOption)
	{
		super();
		
		m_generalOption = generalOption;
	}
	
	@Override
	public void setToDefaults()
	{
		set(SCENE_FILE_NAME, m_generalOption.get(GeneralOption.DEFAULT_SCENE_ABS_PATH));
	}
}
