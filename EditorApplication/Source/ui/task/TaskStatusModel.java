package ui.task;

import java.util.ArrayList;
import java.util.List;

import javax.swing.SwingUtilities;

import ui.event.Event;
import ui.event.EventDispatcher;
import ui.event.EventListener;

public final class TaskStatusModel implements EventDispatcher
{
	private String m_taskName;
	private float m_sampleFrequency;
	private float m_percentageProgress;
	
	private List<EventListener> m_eventListeners;
	
	public TaskStatusModel()
	{
		m_taskName = "default task name";
		m_sampleFrequency = 0.0f;
		m_percentageProgress = 0.0f;
		
		m_eventListeners = new ArrayList<>();
	}
	
	public void setTaskName(String taskName)
	{
		m_taskName = taskName;
		
		Event event = new Event();
		event.m_source = this;
		dispatchEvent(event);
	}
	
	public void setSampleFrequency(float sampleFrequency)
	{
		m_sampleFrequency = sampleFrequency;
		
		Event event = new Event();
		event.m_source = this;
		dispatchEvent(event);
	}
	
	public void setPercentageProgress(float percentageProgress)
	{
		m_percentageProgress = percentageProgress;
		
		Event event = new Event();
		event.m_source = this;
		dispatchEvent(event);
	}
	
	public String getTaskName()
	{
		return m_taskName;
	}
	
	public float getSampleFrequency()
	{
		return m_sampleFrequency;
	}
	
	public float getPercentageProgress()
	{
		return m_percentageProgress;
	}

	@Override
	public void dispatchEvent(Event event)
	{
		SwingUtilities.invokeLater(new Runnable()
		{
			@Override
			public void run()
			{
				for(EventListener listener : m_eventListeners)
				{
					listener.eventTriggered(event);
				}
			}
		});
	}

	@Override
	public void addEventListener(EventListener listener)
	{
		m_eventListeners.add(listener);
	}
}
