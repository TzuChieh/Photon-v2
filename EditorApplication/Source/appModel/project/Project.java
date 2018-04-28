package appModel.project;

import java.io.File;

import appModel.EditorApp;
import appModel.GeneralOption;
import appModel.ManageableResource;
import appModel.event.ProjectEventListener;
import appModel.event.ProjectEventType;
import javafx.application.Platform;
import javafx.concurrent.Task;
import photonApi.FilmInfo;
import photonApi.Frame;
import photonApi.FrameRegion;
import photonApi.FrameStatus;
import photonApi.PhEngine;
import photonApi.PhFrame;
import photonApi.Rectangle;
import photonApi.Statistics;

public final class Project extends ManageableResource
{
	private String                 m_projectName;
	private RenderSetting          m_renderSetting;
	private EditorApp              m_editorApp;
	private ProjectEventDispatcher m_eventDispatcher;
	
	private PhEngine  m_engine;
	private PhFrame   m_finalFrame;
	private PhFrame   m_transientFrame;
	private Frame     m_localFinalFrame;
	
	public Project(String projectName, EditorApp editorApp)
	{
		super();
		
		m_projectName     = projectName;
		m_editorApp       = editorApp;
		m_renderSetting   = new RenderSetting(editorApp.getGeneralOption());
		m_eventDispatcher = new ProjectEventDispatcher();
		
		m_engine          = null;
		m_finalFrame      = null;
		m_transientFrame  = null;
		m_localFinalFrame = new Frame();
	}
	
	public Task<String> createRenderTask()
	{
		return new Task<String>()
		{
			@Override
			protected String call() throws Exception
			{
				opRenderScene();
				return "render task done";
			}
		};
	}
	
	public Task<String> createLoadSceneTask()
	{
		final String sceneFileName = getRenderSetting().get(RenderSetting.SCENE_FILE_NAME);
		
		return new Task<String>()
		{
			@Override
			protected String call() throws Exception
			{
				opLoadSceneFile(sceneFileName);
				return "load scene task done";
			}
		};
	}
	
	public Task<String> createUpdateStaticImageTask()
	{
		return new Task<String>()
		{
			@Override
			protected String call() throws Exception
			{
				opDevelopFilm();
				return "update static image task done";
			}
		};
	}
	
	@Override
	protected void initResource()
	{
		m_engine         = new PhEngine(10);
		m_finalFrame     = new PhFrame(0, 0);
		m_transientFrame = new PhFrame(0, 0);
		
		m_renderSetting.setToDefaults();
	}

	@Override
	protected void freeResource()
	{
		m_engine.dispose();
		m_finalFrame.dispose();
		m_transientFrame.dispose();
	}
	
	public void asyncGetRendererStatistics(Statistics out_statistics)
	{
		m_engine.asyncGetRendererStatistics(out_statistics);
	}
	
	public FrameStatus asyncGetUpdatedFrame(FrameRegion out_frameRegion)
	{
		Rectangle region = new Rectangle();
		FrameStatus status = m_engine.asyncGetUpdatedFrame(m_transientFrame, region);
		if(status != FrameStatus.INVALID)
		{
			out_frameRegion.set(m_transientFrame.copyRegionRgb(region));
		}
		
		return status;
	}
	
	private void opRenderScene()
	{
		EditorApp.printToConsole("rendering scene...");
		
		m_engine.render();
		
		Platform.runLater(() ->
		{
			m_eventDispatcher.notify(ProjectEventType.STATIC_FILM_READY, this);
		});
	}
	
	private void opLoadSceneFile(String filename)
	{
		EditorApp.printToConsole("loading scene file <" + filename + ">...");
		
		File sceneFile = new File(filename);
		m_engine.setWorkingDirectory(sceneFile.getParent());
		m_engine.loadCommand(filename);
		m_engine.update();
		
		FilmInfo info = m_engine.getFilmInfo();
		if(info.widthPx  != m_finalFrame.widthPx() || 
		   info.heightPx != m_finalFrame.heightPx())
		{
			m_finalFrame.dispose();
			m_transientFrame.dispose();
			m_finalFrame = new PhFrame(info.widthPx, info.heightPx);
			m_transientFrame = new PhFrame(info.widthPx, info.heightPx);
		}
		
		Platform.runLater(() ->
		{
			m_eventDispatcher.notify(ProjectEventType.STATIC_SCENE_READY, this);
		});
	}
	
	private void opDevelopFilm()
	{
		EditorApp.printToConsole("developing film...");
		
		m_engine.developFilm(m_finalFrame);
		m_finalFrame.getFullRgb(m_localFinalFrame);
		
		Platform.runLater(() ->
		{
			m_eventDispatcher.notify(ProjectEventType.FINAL_FRAME_READY, this);
		});
	}
	
	public void addListener(ProjectEventType eventType, ProjectEventListener targetListener)
	{
		m_eventDispatcher.addListener(eventType, targetListener);
	}
	
	public void removeListener(ProjectEventListener targetListener)
	{
		m_eventDispatcher.removeListener(targetListener);
	}
	
	public String                 getProjectName()     { return m_projectName;                  }
	public RenderSetting          getRenderSetting()   { return m_renderSetting;                }
	public ProjectEventDispatcher getEventDispatcher() { return m_eventDispatcher;              }
	public Frame                  getLocalFinalFrame() { return m_localFinalFrame;              }
	public FilmInfo               getFilmInfo()        { return m_engine.getFilmInfo();         }
	public GeneralOption          getGeneralOption()   { return m_editorApp.getGeneralOption(); }
}
