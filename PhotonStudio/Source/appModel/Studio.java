package appModel;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import appModel.console.Console;
import appModel.renderProject.RenderProject;
import photonApi.Ph;

public final class Studio extends ManagedResource
{
	private static final Console CONSOLE = new Console(100);
	
	private Map<String, Project> m_projects;
	private GeneralOption        m_generalOption;
	private Project              m_currentProject;
	private StudioStatusView     m_statusView;
	
	public Studio()
	{
		super();
		
		m_projects       = new HashMap<>();
		m_generalOption  = new GeneralOption();
		m_currentProject = null;
		m_statusView     = new StudioStatusView(){};
	}
	
	@Override
	protected void createResource()
	{
		Ph.loadLibrary();
		
		if(Ph.phInit())
		{
			System.out.println("Photon initialized");
		}
		else
		{
			System.err.println("Photon initializing failed");
		}
		
		m_generalOption.load();
	}
	
	@Override
	protected void decomposeResource()
	{
		for(String projectName : m_projects.keySet())
		{
			deleteProject(projectName);
		}
		
		if(Ph.phExit())
		{
			System.out.println("Photon exited");
		}
		else
		{
			System.err.println("Photon exiting failed");
		}
	}
	
	public boolean hasProject(String projectName)
	{
		return m_projects.containsKey(projectName);
	}
	
	public Project getProject(String projectName)
	{
		if(!hasProject(projectName))
		{
			System.err.println("no project named " + projectName);
			return null;
		}
		
		return m_projects.get(projectName);
	}
	
	public RenderProject newRenderProject(String projectName)
	{
		if(hasProject(projectName))
		{
			System.err.println("project already exists");
			return null;
		}
		else
		{
			RenderProject project = new RenderProject(projectName, this);
			project.create();
			
			m_projects.put(projectName, project);
			
			return project;
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
	
	public void setCurrentProject(String projectName)
	{
		Project project = getProject(projectName);
		if(project != null)
		{
			m_currentProject = project;
			m_statusView.showCurrentProject(project);
		}
	}
	
	public void setStatusView(StudioStatusView view)
	{
		m_statusView = view;
	}
	
	public Project getCurrentProject()
	{
		return m_currentProject;
	}
	
	public GeneralOption getGeneralOption()
	{
		return m_generalOption;
	}
	
	public static Console getConsole()
	{
		return CONSOLE;
	}
	
	public static void printToConsole(String message)
	{
		CONSOLE.writeMessage(message);
	}
}
