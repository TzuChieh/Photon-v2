package photonCore;

import photonApi.Ph;

public final class PhUtility
{
	public static void loadTestScene(PhWorld world)
	{
		Ph.phLoadTestScene(world.m_worldId);
	}
}
