package appModel.renderProject;

import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

import javafx.application.Platform;
import photonApi.FrameRegion;
import photonApi.FrameStatus;
import photonApi.Ph;

public class RenderFrameQuery implements Runnable
{
	private static final long MIN_DELAY_MS = 50;
	private static final long MAX_DELAY_MS = 2000;
	
	private RenderProject            m_project;
	private RenderFrameView          m_view;
	
	private ScheduledExecutorService m_executor;
	private long                     m_currentQueryDelayMs;
	private volatile boolean         m_isCancelRequested;
	
	// Constructs a query that can be scheduled by external routines.
	public RenderFrameQuery(RenderProject project, RenderFrameView view)
	{
		m_project             = project;
		m_view                = view;
		
		m_executor            = null;
		m_currentQueryDelayMs = MIN_DELAY_MS;
		m_isCancelRequested   = false;
	}
	
	public void scheduleAdaptively(ScheduledExecutorService executor)
	{
		m_executor            = executor;
		m_currentQueryDelayMs = MIN_DELAY_MS;
		m_isCancelRequested   = false;
		
		m_executor.submit(this);
	}
	
	public void cancelAdaptivelyScheduled()
	{
		m_isCancelRequested = true;
	}

	@Override
	public void run()
	{
		// Scheduled executors will silently consumes errors and stop running,
		// at least we can get an error message by this try-catch block.
		try
		{
			query();
		}
		catch(Throwable t)
		{
			t.printStackTrace();
		}
	}
	
	private void query()
	{
		FrameRegion updatedFrameRegion = new FrameRegion();
		FrameStatus frameStatus = m_project.asyncGetUpdatedFrame(Ph.ATTRIBUTE_LIGHT_ENERGY, updatedFrameRegion);
		
		if(frameStatus != FrameStatus.INVALID)
		{
			Platform.runLater(() -> 
			{
				m_view.showIntermediate(updatedFrameRegion, frameStatus);
			});
		}
		
		if(m_executor != null && !m_isCancelRequested)
		{
			if(frameStatus == FrameStatus.INVALID)
			{
				// increase delay linearly if no updated frame is retrieved
				m_currentQueryDelayMs = Math.min(m_currentQueryDelayMs + MIN_DELAY_MS, MAX_DELAY_MS);
			}
			else
			{
				// decrease delay exponentially if updated frame is retrieved
				m_currentQueryDelayMs = Math.max(m_currentQueryDelayMs / 2, MIN_DELAY_MS);
			}
			
			m_executor.schedule(this, m_currentQueryDelayMs, TimeUnit.MILLISECONDS);
		}
	}
}
