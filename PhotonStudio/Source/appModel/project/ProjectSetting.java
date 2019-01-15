package appModel.project;

import appModel.Setting;
import appModel.SettingGroup;

public class ProjectSetting extends SettingGroup
{
	public static final String PROJECT_NAME = "project-name";
	
	public ProjectSetting()
	{
		super();
		
		add(PROJECT_NAME, "untitled");
	}
	
	public Setting getProjectName()
	{
		return getSetting(PROJECT_NAME);
	}
}
