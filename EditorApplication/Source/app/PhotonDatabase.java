package app;

import java.util.HashMap;
import java.util.Map;

import photonCore.PhEngine;

public final class PhotonDatabase
{
	private Map<String, PhEngine> m_engines;
	
	public PhotonDatabase()
	{
		m_engines = new HashMap<>();
	}
	
	public void addEngine(String name, PhEngine engine)
	{
		m_engines.put(name, engine);
	}
	
	public PhEngine getEngine(String name)
	{
		return m_engines.get(name);
	}
}
