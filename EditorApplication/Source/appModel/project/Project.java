package appModel.project;

import appModel.EditorApp;
import appModel.ManageableResource;
import appModel.event.ProjectEventType;
import javafx.application.Platform;
import photonCore.FrameData;
import photonCore.PhEngine;
import photonCore.PhFrame;

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
		m_renderSetting   = new RenderSetting();
		m_editorApp       = editorApp;
		m_proxy           = null;
		m_eventDispatcher = new ProjectEventDispatcher();
		
		m_engine    = null;
		m_frame     = null;
		m_frameData = new FrameData();
	}
	
	public void opRenderScene()
	{
		m_engine.render();
	}
	
	public void opLoadSceneFile(String filename)
	{
		m_engine.load(filename);
	}
	
	public void opDevelopFilm()
	{
		m_engine.developFilm(m_frame);
		m_frame.getData(m_frameData);
		
		Platform.runLater(() ->
		{
			m_eventDispatcher.notify(ProjectEventType.STATIC_IMAGE_READY, getProxy());
		});
	}
	
	@Override
	protected void initResource()
	{
		m_engine = new PhEngine(4);
		m_frame  = new PhFrame(PhFrame.Type.HDR);
		
		m_proxy = new ProjectProxy(this);
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
