package appGui;

import photonCore.PhEngine;

public class EngineTaskModel
{
	private PhEngine m_engine;
	private String   m_taskName;
	
	public EngineTaskModel()
	{
		m_engine   = new PhEngine(4);
		m_taskName = "untitled task";
	}
	
	public void loadSceneFile(String filename)
	{
		m_engine.load(filename);
	}
	
	public void renderScene()
	{
		m_engine.render();
	}
	
	public String getTaskName()                { return m_taskName;     }
	public void   setTaskName(String taskName) { m_taskName = taskName; }
}
