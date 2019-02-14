package appModel.renderProject;

import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

import javafx.application.Platform;
import photonApi.FrameRegion;
import photonApi.FrameStatus;
import photonApi.PhEngine;
import photonApi.PhFrame;
import photonApi.Rectangle;

/**
 * A query that can peek into frames while they are still rendering. The 
 * result will be presented by the associating view.
 */
public class RenderFrameQuery implements Runnable
{
	private static final long MIN_DELAY_MS = 50;
	private static final long MAX_DELAY_MS = 2000;
	
	private PhEngine                 m_engine;
	private PhFrame                  m_peekingFrame;
	private RenderFrameView          m_view;
	
	private ScheduledExecutorService m_executor;
	private long                     m_currentQueryDelayMs;
	private volatile int             m_channelIndex;
	private volatile boolean         m_isCancelRequested;
	
	/**
	 * Constructs a query that can be scheduled by external routines.
	 */
	public RenderFrameQuery(
		PhEngine        engine, 
		PhFrame         peekingFrame,
		RenderFrameView view)
	{
		m_engine              = engine;
		m_peekingFrame        = peekingFrame;
		m_view                = view;
		
		m_executor            = null;
		m_currentQueryDelayMs = MIN_DELAY_MS;
		m_channelIndex        = 0;
		m_isCancelRequested   = false;
	}
	
	public void scheduleAdaptively(ScheduledExecutorService executor)
	{
		m_executor            = executor;
		m_currentQueryDelayMs = MIN_DELAY_MS;
		m_isCancelRequested   = false;
		
		m_executor.submit(this);
	}
	
	/**
	 * Thread-safe
	 */
	public void setChannel(int channelIndex)
	{
		assert(channelIndex >= 0);
		
		m_channelIndex = channelIndex;
	}
	
	/**
	 * Thread-safe
	 */
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
		Rectangle updatedRegion = new Rectangle();
		FrameStatus status = m_engine.asyncPeekUpdatedFrame(m_channelIndex, m_peekingFrame, updatedRegion);
		if(status != FrameStatus.INVALID)
		{
			FrameRegion updatedFrameRegion = m_peekingFrame.copyRegionRgb(updatedRegion);
			Platform.runLater(() -> 
			{
				m_view.showPeeked(updatedFrameRegion, status);
			});
		}
		
		if(m_executor != null && !m_isCancelRequested)
		{
			if(status == FrameStatus.INVALID)
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
