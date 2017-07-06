package appModel.project;

import appModel.GeneralOption;
import appModel.SettingGroup;

public final class RenderSetting extends SettingGroup
{
	public static final int SCENE_FILE_NAME = 1;
	
	private GeneralOption m_generalOption;
	
	public RenderSetting(GeneralOption generalOption)
	{
		super();
		
		m_generalOption = generalOption;
	}
	
	@Override
	public void setToDefaults()
	{
		set(SCENE_FILE_NAME, m_generalOption.get(GeneralOption.DEFAULT_SCENE_PATH));
	}
}
