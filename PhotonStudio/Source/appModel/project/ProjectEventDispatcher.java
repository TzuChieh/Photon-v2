package appModel.project;

import java.util.ArrayList;
import java.util.EnumMap;
import java.util.List;
import java.util.Map;

import appModel.event.ProjectEvent;
import appModel.event.ProjectEventListener;
import appModel.event.ProjectEventType;

public final class ProjectEventDispatcher
{
	private Map<ProjectEventType, List<ProjectEventListener>> m_listenersMap;
	
	public ProjectEventDispatcher()
	{
		m_listenersMap = new EnumMap<>(ProjectEventType.class);
	}
	
	protected void notify(ProjectEventType eventType, RenderProject eventSource)
	{
		List<ProjectEventListener> listeners = m_listenersMap.get(eventType);
		if(listeners != null)
		{
			for(ProjectEventListener listener : listeners)
			{
				ProjectEvent event = new ProjectEvent();
				event.type   = eventType;
				event.source = eventSource;
				
				listener.onEventOccurred(event);
			}
		}
	}
	
	public void addListener(ProjectEventType eventType, ProjectEventListener targetListener)
	{
		List<ProjectEventListener> listeners = m_listenersMap.get(eventType);
		if(listeners == null)
		{
			listeners = new ArrayList<>();
			m_listenersMap.put(eventType, listeners);
		}
		
		listeners.add(targetListener);
	}
	
	public void removeListener(ProjectEventListener targetListener)
	{
		for(Map.Entry<ProjectEventType, List<ProjectEventListener>> entry : m_listenersMap.entrySet())
		{
			List<ProjectEventListener> listeners = entry.getValue();
			while(listeners.remove(targetListener));
		}
	}
}
