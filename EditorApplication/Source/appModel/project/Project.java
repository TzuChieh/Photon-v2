package appModel.project;

import appModel.EditorApp;
import appModel.ManageableResource;
import appModel.event.ProjectEventType;
import javafx.application.Platform;
import photonApi.FilmInfo;
import photonApi.Frame;
import photonApi.FrameRegion;
import photonApi.FrameStatus;
import photonApi.PhEngine;
import photonApi.PhFrame;
import photonApi.Rectangle;

public final class Project extends ManageableResource
{
	private String                 m_projectName;
	private RenderSetting          m_renderSetting;
	private EditorApp              m_editorApp;
	private ProjectProxy           m_proxy;
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
		m_proxy           = null;
		m_eventDispatcher = new ProjectEventDispatcher();
		
		m_engine          = null;
		m_finalFrame      = null;
		m_transientFrame  = null;
		m_localFinalFrame = new Frame();
	}
	
	public void opRenderScene()
	{
		EditorApp.printToConsole("rendering scene...");
		
		m_engine.render();
		
		Platform.runLater(() ->
		{
			m_eventDispatcher.notify(ProjectEventType.STATIC_FILM_READY, getProxy());
		});
	}
	
	public void opLoadSceneFile(String filename)
	{
		EditorApp.printToConsole("loading scene file <" + filename + ">...");
		
		m_engine.load(filename);
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
			m_eventDispatcher.notify(ProjectEventType.STATIC_SCENE_READY, getProxy());
		});
	}
	
	public void opDevelopFilm()
	{
		EditorApp.printToConsole("developing film...");
		
		m_engine.developFilm(m_finalFrame);
		m_finalFrame.getFullRgb(m_localFinalFrame);
		
		Platform.runLater(() ->
		{
			m_eventDispatcher.notify(ProjectEventType.FINAL_FRAME_READY, getProxy());
		});
	}
	
	@Override
	protected void initResource()
	{
		m_engine         = new PhEngine(6);
		m_finalFrame     = new PhFrame(0, 0);
		m_transientFrame = new PhFrame(0, 0);
		m_proxy          = new ProjectProxy(this);
		
		m_renderSetting.setToDefaults();
	}

	@Override
	protected void freeResource()
	{
		m_engine.dispose();
		m_finalFrame.dispose();
		m_transientFrame.dispose();
	}
	
	public float asyncQueryParametricProgress()
	{
		return m_engine.asyncQueryPercentageProgress() / 100.0f;
	}
	
	public float asyncQuerySamplingFrequency()
	{
		return m_engine.asyncQuerySampleFrequency();
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
	
	public String                 getProjectName()     { return m_projectName;          }
	public RenderSetting          getRenderSetting()   { return m_renderSetting;        }
	public ProjectProxy           getProxy()           { return m_proxy;                }
	public ProjectEventDispatcher getEventDispatcher() { return m_eventDispatcher;      }
	public Frame                  getLocalFinalFrame() { return m_localFinalFrame;      }
	public FilmInfo               getFilmInfo()        { return m_engine.getFilmInfo(); }
}
