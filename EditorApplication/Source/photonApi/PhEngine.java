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
	
	public void load(String filename)
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
		Ph.phDevelopFilm(m_engineId, frame.m_frameId);
	}
	
	public float queryPercentageProgress()
	{
		FloatRef progress = new FloatRef();
		Ph.phQueryRendererPercentageProgress(m_engineId, progress);
		return progress.m_value;
	}
	
	public float querySampleFrequency()
	{
		FloatRef frequency = new FloatRef();
		Ph.phQueryRendererSampleFrequency(m_engineId, frequency);
		return frequency.m_value;
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
