package appModel;

import java.util.ArrayList;
import java.util.List;

import appModel.event.LifeCycleEvent;
import appModel.event.LifeCycleListener;

public abstract class ManagedResource
{
	List<LifeCycleListener> m_lifeCycleListeners;
	
	protected ManagedResource()
	{
		m_lifeCycleListeners = new ArrayList<>();
	}
	
	protected abstract void createResource();
	protected abstract void decomposeResource();
	
	public void create()
	{
		createResource();
		
		for(LifeCycleListener listener : m_lifeCycleListeners)
		{
			LifeCycleEvent event = new LifeCycleEvent();
			event.source = this;
			
			listener.onCreate(event);
		}
	}
	
	public void decompose()
	{
		for(LifeCycleListener listener : m_lifeCycleListeners)
		{
			LifeCycleEvent event = new LifeCycleEvent();
			event.source = this;
			
			listener.onDecompose(event);
		}
		
		decomposeResource();
	}
	
	public void addLifeCycleListener(LifeCycleListener listener)
	{
		m_lifeCycleListeners.add(listener);
	}
}
