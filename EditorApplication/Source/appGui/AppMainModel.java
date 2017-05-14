package appGui;

import java.util.HashMap;
import java.util.Map;

import photonCore.PhEngine;

public final class AppMainModel
{
	private Map<String, EngineTaskModel> m_engineTaskModels;
	
	public AppMainModel()
	{
		m_engineTaskModels = new HashMap<>();
	}
	
	public void addEngineTaskModel(String name, EngineTaskModel model)
	{
		m_engineTaskModels.put(name, model);
	}
	
	public EngineTaskModel getEngineTaskModel(String name)
	{
		return m_engineTaskModels.get(name);
	}
}
