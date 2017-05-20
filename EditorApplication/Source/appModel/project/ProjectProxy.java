package appModel.project;

import appModel.event.ProjectEventListener;
import appModel.event.ProjectEventType;
import javafx.concurrent.Task;
import photonCore.FrameData;

public final class ProjectProxy
{
	private Project m_project;
	
	public ProjectProxy(Project project)
	{
		m_project = project;
	}
	
	public Task<String> createTask(TaskType type)
	{
		switch(type)
		{
		case RENDER:
			return createRenderTask();
		case LOAD_SCENE:
			return createLoadSceneTask();
		case UPDATE_STATIC_IMAGE:
			return createUpdateStaticImageTask();
		}
		
		System.err.println("unknown task type");
		return null;
	}
	
	public RenderSetting getRenderSetting()
	{
		return m_project.getRenderSetting();
	}
	
	private Task<String> createRenderTask()
	{
		return new Task<String>()
		{
			@Override
			protected String call() throws Exception
			{
				m_project.opRenderScene();
				return "render task done";
			}
		};
	}
	
	private Task<String> createLoadSceneTask()
	{
		final String sceneFileName = getRenderSetting().get(RenderSetting.SCENE_FILE_NAME);
		
		return new Task<String>()
		{
			@Override
			protected String call() throws Exception
			{
				m_project.opLoadSceneFile(sceneFileName);
				return "load scene task done";
			}
		};
	}
	
	private Task<String> createUpdateStaticImageTask()
	{
		return new Task<String>()
		{
			@Override
			protected String call() throws Exception
			{
				m_project.opDevelopFilm();
				return "update static image task done";
			}
		};
	}
	
	public float queryParametricProgress()
	{
		return m_project.queryParametricProgress();
	}
	
	public float querySamplingFrequency()
	{
		return m_project.querySamplingFrequency();
	}
	
	public void addListener(ProjectEventType eventType, ProjectEventListener targetListener)
	{
		m_project.getEventDispatcher().addListener(eventType, targetListener);
	}
	
	public void removeListener(ProjectEventListener targetListener)
	{
		m_project.getEventDispatcher().removeListener(targetListener);
	}
	
	public boolean isValid()
	{
		return m_project != null;
	}
	
	public FrameData getStaticImageData() { return m_project.getFrameData(); }
}
