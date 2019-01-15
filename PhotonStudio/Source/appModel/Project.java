package appModel;

import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;

public abstract class Project extends ManagedResource
{
	private ProjectSetting m_projectSetting;
	private ProjectLogView m_logView;
	
	protected Project(String projectName)
	{
		m_projectSetting = new ProjectSetting();
		m_logView        = new ProjectLogView(){};
		
		m_projectSetting.getProjectName().setValue(projectName);
	}
	
	@Override
	protected abstract void createResource();
	
	@Override
	protected abstract void decomposeResource();
	
	public void save(Path path)
	{
		List<SettingGroup> allSettings = getAllSettings();
		
		SettingGroup settings = new SettingGroup();
		for(SettingGroup settingGroup : allSettings)
		{
			settings.add(settingGroup);
		}
		settings.save(path);
	}
	
	public void load(Path path)
	{
		// TODO
		// to properly save/load all settings we need to be able to distinguish different groups...
	}
	
	public List<SettingGroup> getAllSettings()
	{
		List<SettingGroup> settings = new ArrayList<>();
		settings.add(m_projectSetting);
		return settings;
	}
	
	public ProjectSetting getProjectSetting()
	{
		return m_projectSetting;
	}
	
	protected ProjectLogView getLogView()
	{
		return m_logView;
	}
	
	public void setLogView(ProjectLogView view)
	{
		m_logView = view;
	}
}
