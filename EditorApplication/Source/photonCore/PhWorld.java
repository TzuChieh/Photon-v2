package photonCore;

import photonApi.LongRef;
import photonApi.Ph;

public final class PhWorld
{
	protected long m_worldId;
	
	public PhWorld()
	{
		LongRef worldId = new LongRef();
		Ph.phCreateWorld(worldId);
		m_worldId = worldId.m_value;
	}
	
	public void cook()
	{
		Ph.phCookWorld(m_worldId);
	}
}
