package appModel.project;

import java.io.File;

import appModel.Studio;
import appModel.GeneralOption;
import appModel.ManagedResource;
import appModel.event.ProjectEventListener;
import appModel.event.ProjectEventType;
import javafx.application.Platform;
import javafx.concurrent.Task;
import photonApi.FilmInfo;
import photonApi.Frame;
import photonApi.FrameRegion;
import photonApi.FrameStatus;
import photonApi.Ph;
import photonApi.PhEngine;
import photonApi.PhFrame;
import photonApi.Rectangle;
import photonApi.RenderState;
import photonApi.Statistics;

public final class Project extends ManagedResource
{
	private String                 m_projectName;
	private RenderSetting          m_renderSetting;
	private Studio                 m_studio;
	private ProjectEventDispatcher m_eventDispatcher;
	
	private PhEngine m_engine;
	private PhFrame  m_finalFrame;
	private PhFrame  m_transientFrame;
	private Frame    m_localFinalFrame;
	
	private int m_numThreads;
	
	public Project(String projectName, Studio studio)
	{
		super();
		
		m_projectName     = projectName;
		m_studio       = studio;
		m_renderSetting   = new RenderSetting(studio.getGeneralOption());
		m_eventDispatcher = new ProjectEventDispatcher();
		
		m_engine          = null;
		m_finalFrame      = null;
		m_transientFrame  = null;
		m_localFinalFrame = new Frame();
		
		m_numThreads = 1;
	}
	
	public Runnable newRenderTask()
	{
		return () ->
		{
			Studio.printToConsole("rendering scene...");
			
			m_engine.render();
			
			Platform.runLater(() ->
			{
				m_eventDispatcher.notify(ProjectEventType.STATIC_FILM_READY, this);
			});
		};
	}
	
	public Runnable newLoadSceneTask()
	{
		final String sceneFilePath = getRenderSetting().get(RenderSetting.SCENE_FILE_PATH);
		return () ->
		{
			Studio.printToConsole("loading scene file <" + sceneFilePath + ">...");
			
			File sceneFile = new File(sceneFilePath);
			m_engine.setNumRenderThreads(m_numThreads);
			m_engine.setWorkingDirectory(sceneFile.getParent());
			m_engine.loadCommand(sceneFilePath);
			
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
		};
	}
	
	public Runnable newUpdateStaticImageTask()
	{
		return () ->
		{
			Studio.printToConsole("developing film...");
			
			m_engine.developFilm(m_finalFrame);
			m_finalFrame.getFullRgb(m_localFinalFrame);
			
			Platform.runLater(() ->
			{
				m_eventDispatcher.notify(ProjectEventType.FINAL_FRAME_READY, this);
			});
		};
	}
	
	public void setNumThreads(int numThreads)
	{
		m_numThreads = numThreads;
	}
	
	@Override
	protected void initResource()
	{
		m_engine         = new PhEngine(m_numThreads);
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
		return asyncGetUpdatedFrame(Ph.ATTRIBUTE_LIGHT_ENERGY, out_frameRegion);
	}
	
	public FrameStatus asyncGetUpdatedFrame(int attribute, FrameRegion out_frameRegion)
	{
		Rectangle region = new Rectangle();
		FrameStatus status = m_engine.asyncGetUpdatedFrame(attribute, m_transientFrame, region);
		if(status != FrameStatus.INVALID)
		{
			out_frameRegion.set(m_transientFrame.copyRegionRgb(region));
		}
		
		return status;
	}
	
	public RenderState asyncGetRenderState()
	{
		return m_engine.asyncGetRenderState();
	}
	
	public void addListener(ProjectEventType eventType, ProjectEventListener targetListener)
	{
		m_eventDispatcher.addListener(eventType, targetListener);
	}
	
	public void removeListener(ProjectEventListener targetListener)
	{
		m_eventDispatcher.removeListener(targetListener);
	}
	
	public String getIntegerRenderStateName(int index)
	{
		return m_engine.getIntegerRenderStateName(index);
	}
	
	public String getRealRenderStateName(int index)
	{
		return m_engine.getRealRenderStateName(index);
	}
	
	public String                 getProjectName()     { return m_projectName;                  }
	public RenderSetting          getRenderSetting()   { return m_renderSetting;                }
	public ProjectEventDispatcher getEventDispatcher() { return m_eventDispatcher;              }
	public Frame                  getLocalFinalFrame() { return m_localFinalFrame;              }
	public FilmInfo               getFilmInfo()        { return m_engine.getFilmInfo();         }
	public GeneralOption          getGeneralOption()   { return m_studio.getGeneralOption(); }
}
