package photonApi;

import java.io.BufferedReader;
import java.io.FileReader;

public final class PhEngine
{
	protected final long m_engineId;
	
	private int m_numRenderThreads;	
	
	public PhEngine(int numRenderThreads)
	{
		LongRef engineId = new LongRef();
		Ph.phCreateEngine(engineId, numRenderThreads);
		
		m_engineId = engineId.m_value;
		
		m_numRenderThreads = numRenderThreads;
	}
	
	public void enterCommand(String commandFragment)
	{
		Ph.phEnterCommand(m_engineId, commandFragment);
	}
	
	public void setWorkingDirectory(String workingDirectory)
	{
		Ph.phSetWorkingDirectory(m_engineId, workingDirectory);
	}
	
	public void loadCommand(String filename)
	{
		try
		{
			BufferedReader reader = new BufferedReader(new FileReader(filename));
		    String line = null;
		    while((line = reader.readLine()) != null)
		    {
		    	enterCommand(line + '\n');
		    }
		    enterCommand("->");
		    
		    reader.close();
		}
		catch(Exception e)
		{
			e.printStackTrace();
		}
	}
	
	public void update()
	{
		Ph.phUpdate(m_engineId);
	}
	
	public void render()
	{
		Ph.phRender(m_engineId);
	}
	
	public FrameInfo getFrameInfo()
	{
		IntRef widthPx  = new IntRef();
		IntRef heightPx = new IntRef();
		Ph.phGetRenderDimension(m_engineId, widthPx, heightPx);
		
		FrameInfo info = new FrameInfo();
		info.widthPx  = widthPx.m_value;
		info.heightPx = heightPx.m_value;
		return info;
	}
	
	public void aquireFrame(PhFrame frame)
	{
		Ph.phAquireFrame(m_engineId, 0, frame.m_frameId);
	}
	
	public void asyncGetRendererStatistics(Statistics out_statistics)
	{
		FloatRef progress  = new FloatRef();
		FloatRef frequency = new FloatRef();
		Ph.phAsyncGetRendererStatistics(m_engineId, progress, frequency);
		
		out_statistics.percentageProgress = progress.m_value;
		out_statistics.samplesPerSecond   = frequency.m_value;
	}
	
	public FrameStatus asyncPollUpdatedFrameRegion(Rectangle out_updatedRegion)
	{
		IntRef xPx = new IntRef();
		IntRef yPx = new IntRef();
		IntRef wPx = new IntRef();
		IntRef hPx = new IntRef();
		final int pollState = Ph.phAsyncPollUpdatedFrameRegion(m_engineId, xPx, yPx, wPx, hPx);
		
		out_updatedRegion.x = xPx.m_value;
		out_updatedRegion.y = yPx.m_value;
		out_updatedRegion.w = wPx.m_value;
		out_updatedRegion.h = hPx.m_value;
		
		switch(pollState)
		{
		case Ph.FILM_REGION_STATUS_INVALID:  return FrameStatus.INVALID;
		case Ph.FILM_REGION_STATUS_UPDATING: return FrameStatus.UPDATING;
		case Ph.FILM_REGION_STATUS_FINISHED: return FrameStatus.FINISHED;
		default:                             return FrameStatus.INVALID;
		}
	}
	
	public void asyncPeekFrame(int channelIndex, PhFrame frame)
	{
		Ph.phAsyncPeekFrame(
			m_engineId, 
			channelIndex, 
			0, 0, frame.m_widthPx, frame.m_heightPx,
			frame.m_frameId);
	}
	
	public FrameStatus asyncPeekUpdatedFrame(int channelIndex, PhFrame frame, Rectangle out_updatedRegion)
	{
		final FrameStatus pollStatus = asyncPollUpdatedFrameRegion(out_updatedRegion);
		
		if(pollStatus != FrameStatus.INVALID)
		{
			Ph.phAsyncPeekFrame(
				m_engineId, 
				channelIndex, 
				out_updatedRegion.x, 
				out_updatedRegion.y, 
				out_updatedRegion.w, 
				out_updatedRegion.h, 
				frame.m_frameId);
		}
		
		return pollStatus;
	}
	
	public void dispose()
	{
		Ph.phDeleteEngine(m_engineId);
	}
	
	public void setNumRenderThreads(int numRenderThreads)
	{
		Ph.phSetNumRenderThreads(m_engineId, numRenderThreads);
		
		m_numRenderThreads = numRenderThreads;
	}
	
	public int getNumRenderThreads()
	{
		return m_numRenderThreads;
	}
	
	public RenderState asyncGetRenderState()
	{
		RenderState state = new RenderState();
		Ph.phAsyncGetRendererState(m_engineId, state);
		return state;
	}
	
	public ObservableRenderData getObservableRenderData()
	{
		ObservableRenderData data = new ObservableRenderData();
		Ph.phGetObservableRenderData(m_engineId, data);
		return data;
	}
	
	@Override
	protected void finalize()
	{
		// TODO: check whether the engine is properly deleted, if not, deleted it
	}
}
