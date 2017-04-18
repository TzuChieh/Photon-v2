package photonApi;

public final class Ph
{
	// Notice: Photon-v2 library functions should not be called from threads other than the main thread.
	
	static
	{
		System.loadLibrary("Engine/Lib/Engine");
		System.loadLibrary("JNI/JNI");
	}
	
	public static final int PH_HDR_FRAME_TYPE = 50;
	
	// core
	
	public static native boolean phInit();
	public static native boolean phExit();
	
	public static native void phCreateEngine(LongRef out_engineId, int numRenderThreads);
	public static native void phEnterCommand(long engineId, String commandFragment);
	public static native void phRender(long engineId);
	public static native void phDevelopFilm(long engineId, long frameId);
	public static native void phDeleteEngine(long engineId);
	
	public static native void phCreateFrame(LongRef out_frameId, int frameType);
	public static native void phGetFrameData(long frameId, FloatArrayRef out_pixelData, IntRef out_widthPx, IntRef out_heightPx, IntRef out_nPixelComponents);
	public static native void phDeleteFrame(long frameId);
	
	// query
	
	public static native void phQueryRendererPercentageProgress(long engineId, FloatRef out_progress);
	public static native void phQueryRendererSampleFrequency(long engineId, FloatRef out_frequency);
}
