package photonApi;

import static photonApi.Ph.*;

public class PhTest
{
	public PhTest() 
	{
		LongRef rendererId = new LongRef();
		phCreateRenderer(rendererId, PH_BRUTE_FORCE_RENDERER_TYPE);
		phDeleteRenderer(rendererId.m_value);
	}
}
