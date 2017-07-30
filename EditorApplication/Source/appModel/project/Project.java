package appModel.project;

import appModel.EditorApp;
import appModel.ManageableResource;
import appModel.event.ProjectEventType;
import javafx.application.Platform;
import photonApi.FilmInfo;
import photonApi.FrameData;
import photonApi.PhEngine;
import photonApi.PhFrame;

public final class Project extends ManageableResource
{
	private String                 m_projectName;
	private RenderSetting          m_renderSetting;
	private EditorApp              m_editorApp;
	private ProjectProxy           m_proxy;
	private ProjectEventDispatcher m_eventDispatcher;
	
	private PhEngine  m_engine;
	private PhFrame   m_frame;
	private FrameData m_frameData;
	
	public Project(String projectName, EditorApp editorApp)
	{
		super();
		
		m_projectName     = projectName;
		m_editorApp       = editorApp;
		m_renderSetting   = new RenderSetting(editorApp.getGeneralOption());
		m_proxy           = null;
		m_eventDispatcher = new ProjectEventDispatcher();
		
		m_engine    = null;
		m_frame     = null;
		m_frameData = new FrameData();
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
		
		Platform.runLater(() ->
		{
			m_eventDispatcher.notify(ProjectEventType.STATIC_SCENE_READY, getProxy());
		});
	}
	
	public void opDevelopFilm()
	{
		EditorApp.printToConsole("developing film...");
		
		FilmInfo info = m_engine.getFilmInfo();
		if(info.widthPx  != m_frame.widthPx() || 
		   info.heightPx != m_frame.heightPx())
		{
			m_frame.dispose();
			m_frame = new PhFrame(info.widthPx, info.heightPx);
		}
		
		m_engine.developFilm(m_frame);
		m_frame.getRgbData(m_frameData);
		
		Platform.runLater(() ->
		{
			m_eventDispatcher.notify(ProjectEventType.STATIC_FRAME_READY, getProxy());
		});
	}
	
	@Override
	protected void initResource()
	{
		m_engine = new PhEngine(6);
		m_frame  = new PhFrame(0, 0);
		m_proxy  = new ProjectProxy(this);
		
		m_renderSetting.setToDefaults();
	}

	@Override
	protected void freeResource()
	{
		m_engine.dispose();
		m_frame.dispose();
	}
	
	public float queryParametricProgress()
	{
		return m_engine.queryPercentageProgress() / 100.0f;
	}
	
	public float querySamplingFrequency()
	{
		return m_engine.querySampleFrequency();
	}
	
	public String                 getProjectName()     { return m_projectName;     }
	public RenderSetting          getRenderSetting()   { return m_renderSetting;   }
	public ProjectProxy           getProxy()           { return m_proxy;           }
	public ProjectEventDispatcher getEventDispatcher() { return m_eventDispatcher; }
	public FrameData              getFrameData()       { return m_frameData;       }
}
