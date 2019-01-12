package photonApi;

public class RenderState
{
	public long[]  integerStates;
	public float[] realStates;
	
	public RenderState()
	{
		integerStates = new long[3];
		realStates    = new float[3];
	}
}
