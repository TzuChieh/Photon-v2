package appModel;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import appModel.console.Console;
import appModel.project.Project;
import photonApi.Ph;

public final class Studio extends ManagedResource
{
	private static final Console CONSOLE = new Console(100);
	
	private Map<String, Project> m_projects;
	private GeneralOption        m_generalOption;
	private List<Editor>         m_editors;
	
	public Studio()
	{
		super();
		
		m_projects      = new HashMap<>();
		m_generalOption = new GeneralOption();
		m_editors       = new ArrayList<>();
	}
	
	@Override
	protected void initResource()
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
	protected void freeResource()
	{
		for(String projectName : m_projects.keySet())
		{
			deleteProject(projectName);
		}
		
		for(Editor editor : m_editors)
		{
			deleteEditor(editor);
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
		if(hasProject(projectName))
		{
			System.err.println("no project named " + projectName);
			return null;
		}
		
		return m_projects.get(projectName);
	}
	
	public Project newProject(String projectName)
	{
		if(hasProject(projectName))
		{
			System.err.println("project already exists");
			return null;
		}
		else
		{
			Project project = new Project(projectName, this);
			project.create();
			
			m_projects.put(projectName, project);
			
			return project;
		}
	}
	
	public Editor newEditor()
	{
		Editor editor = new Editor();
		editor.create();
		
		m_editors.add(editor);
		
		return editor;
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
			for(Editor editor : m_editors)
			{
				if(editor.getProject() == project)
				{
					editor.setProject(null);
				}
			}
			
			project.decompose();
			m_projects.remove(projectName, project);
		}
	}
	
	public void deleteEditor(Editor editor)
	{
		editor.decompose();
		
		m_editors.remove(editor);
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
