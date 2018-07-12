package photonApi;

import java.io.BufferedReader;
import java.io.FileReader;

public final class PhEngine
{
	private int m_numRenderThreads;
	
	protected final long m_engineId;
	
	public PhEngine(int numRenderThreads)
	{
		m_numRenderThreads = numRenderThreads;
		
		LongRef engineId = new LongRef();
		Ph.phCreateEngine(engineId, numRenderThreads);
		m_engineId = engineId.m_value;
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
	
	public FilmInfo getFilmInfo()
	{
		IntRef widthPx  = new IntRef();
		IntRef heightPx = new IntRef();
		Ph.phGetFilmDimension(m_engineId, widthPx, heightPx);
		
		FilmInfo info = new FilmInfo();
		info.widthPx  = widthPx.m_value;
		info.heightPx = heightPx.m_value;
		return info;
	}
	
	public void developFilm(PhFrame frame)
	{
		Ph.phDevelopFilm(m_engineId, frame.m_frameId, Ph.ATTRIBUTE_LIGHT_ENERGY);
	}
	
	public void asyncGetRendererStatistics(Statistics out_statistics)
	{
		FloatRef progress  = new FloatRef();
		FloatRef frequency = new FloatRef();
		Ph.phAsyncGetRendererStatistics(m_engineId, progress, frequency);
		
		out_statistics.percentageProgress = progress.m_value;
		out_statistics.samplesPerSecond   = frequency.m_value;
	}
	
	public FrameStatus asyncGetUpdatedFrame(PhFrame out_frame, Rectangle out_updatedRegion)
	{
		return asyncGetUpdatedFrame(Ph.ATTRIBUTE_LIGHT_ENERGY, out_frame, out_updatedRegion);
	}
	
	public FrameStatus asyncGetUpdatedFrame(int attribute, PhFrame out_frame, Rectangle out_updatedRegion)
	{
		IntRef xPx = new IntRef();
		IntRef yPx = new IntRef();
		IntRef wPx = new IntRef();
		IntRef hPx = new IntRef();
		int pollState = Ph.phAsyncPollUpdatedFilmRegion(m_engineId, xPx, yPx, wPx, hPx);
		if(pollState == Ph.FILM_REGION_STATUS_INVALID)
		{
			return FrameStatus.INVALID;
		}
		
		Ph.phAsyncDevelopFilmRegion(m_engineId, out_frame.m_frameId, 
		                            xPx.m_value, yPx.m_value, wPx.m_value, hPx.m_value, attribute);
		
		out_updatedRegion.x = xPx.m_value;
		out_updatedRegion.y = yPx.m_value;
		out_updatedRegion.w = wPx.m_value;
		out_updatedRegion.h = hPx.m_value;
		
		switch(pollState)
		{
		case Ph.FILM_REGION_STATUS_UPDATING: return FrameStatus.UPDATING;
		case Ph.FILM_REGION_STATUS_FINISHED: return FrameStatus.FINISHED;
		default:                             return FrameStatus.INVALID;
		}
	}
	
	public void dispose()
	{
		Ph.phDeleteEngine(m_engineId);
	}
	
	public void setNumRenderThreads(int numRenderThreads)
	{
		m_numRenderThreads = numRenderThreads;
		
		// TODO: set
	}
	
	@Override
	protected void finalize()
	{
		// TODO: check whether the engine is properly deleted, if not, deleted it
	}
}
