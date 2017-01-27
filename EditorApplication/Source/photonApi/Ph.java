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
	
	public static native boolean phStart();
	public static native void    phExit();
	
	public static native void    phCreateRenderer(LongRef out_rendererId, int numThreads);
	public static native void    phRender(long rendererId, long descriptionId);
	public static native void    phDeleteRenderer(long rendererId);
	
	public static native void    phCreateFrame(LongRef out_frameId, int frameType);
	public static native void    phGetFrameData(long frameId, FloatArrayRef out_pixelData, IntRef out_widthPx, IntRef out_heightPx, IntRef out_nPixelComponents);
	public static native void    phDeleteFrame(long frameId);
	
	public static native void    phCreateDescription(LongRef out_descriptionId);
	public static native void    phLoadDescription(long descriptionId, String filename);
	public static native void    phUpdateDescription(long descriptionId);
	public static native void    phDevelopFilm(long descriptionId, long frameId);
	public static native void    phDeleteDescription(long descriptionId);
	
	// query
	
	public static native void phQueryRendererPercentageProgress(long rendererId, FloatRef out_progress);
	public static native void phQueryRendererSampleFrequency(long rendererId, FloatRef out_frequency);
}
