package appModel.project;

import appModel.SettingGroup;

public final class RenderSetting extends SettingGroup
{
	public static final int SCENE_FILE_NAME = 1;
	
	public RenderSetting()
	{
		super();
		
		set(SCENE_FILE_NAME, "");
	}
}
