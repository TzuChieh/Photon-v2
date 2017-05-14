package appModel;

import java.util.HashMap;
import java.util.Map;

import appModel.project.Project;
import photonApi.Ph;

public final class EditorApp extends ManageableResource
{
	private Map<String, Project> m_projects;
	
	// TODO: engines here
	private Map<String, Project> m_projects;
	
	public EditorApp()
	{
		super();
		
		m_projects = new HashMap<>();
	}
	
	@Override
	protected void initResource()
	{
		if(Ph.phInit())
			System.out.println("Photon initialized");
		else
			System.err.println("Photon initializing failed");
	}
	
	@Override
	protected void freeResource()
	{
		if(Ph.phExit())
			System.out.println("Photon exited");
		else
			System.err.println("Photon exiting failed");
	}
	
	public Project getProject(String projectName)
	{
		return m_projects.get(projectName);
	}
	
	public void addProject(String projectName, Project project)
	{
		if(m_projects.get(projectName) != null)
		{
			System.err.println("project name already exists");
		}
		else
		{
			m_projects.put(projectName, project);
		}
	}
}
