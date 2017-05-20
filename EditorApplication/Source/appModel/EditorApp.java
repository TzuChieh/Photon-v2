package appModel;

import java.util.HashMap;
import java.util.Map;

import appModel.project.Project;
import appModel.project.ProjectProxy;
import photonApi.Ph;

public final class EditorApp extends ManageableResource
{
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
	
	public ProjectProxy getProject(String projectName)
	{
		return new ProjectProxy(m_projects.get(projectName));
	}
	
	public ProjectProxy createProject(String projectName)
	{
		if(m_projects.get(projectName) != null)
		{
			System.err.println("project already exists");
			return null;
		}
		else
		{
			Project project = new Project(projectName, this);
			project.create();
			m_projects.put(projectName, project);
			return new ProjectProxy(project);
		}
	}
	
	public void deleteProject(String projectName)
	{
		Project project = m_projects.get(projectName);
		if(project == null)
		{
			System.err.println("project does not exist");
		}
		else
		{
			project.decompose();
			m_projects.remove(projectName, project);
		}
	}
}
