package appModel.project;

import appModel.ManageableResource;
import photonCore.PhEngine;

public final class Project extends ManageableResource
{
	private String m_projectName;
//	private PhEngine m_engine;
	private RenderSetting m_renderSetting;
	
	public Project(String projectName)
	{
		super();
		
		m_projectName = projectName;
		m_engine = null;
		m_renderSetting = new RenderSetting();
	}
	
	@Override
	protected void initResource()
	{
		m_engine = new PhEngine(4);
	}

	@Override
	protected void freeResource()
	{
		m_engine.dispose();
	}
	
	public String        getProjectName()   { return m_projectName;   }
	public RenderSetting getRenderSetting() { return m_renderSetting; }
}
