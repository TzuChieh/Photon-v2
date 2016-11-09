package photonApi;

public final class Ph
{
	// Notice: Photon-v2 library functions should not be called from threads other than the main thread.
	
	static
	{
		System.loadLibrary("Engine/Lib/Engine");
		System.loadLibrary("JNI/JNI");
	}
	
	// core
	
	public static final int PH_BRUTE_FORCE_RENDERER_TYPE = 1;
	public static final int PH_IMPORTANCE_RENDERER_TYPE  = 2;
	
	public static native boolean phStart();
	public static native void    phExit();
	public static native void    phCreateRenderer(LongRef out_rendererId, int rendererType);
	public static native void    phDeleteRenderer(long rendererId);
	
	// some tests
	
	public static native void printTestMessage();
	public static native void genTestHdrFrame(FloatArrayRef out_pixelData, IntRef out_widthPx, IntRef out_heightPx);
	
	// image related
	
	public static native void phCreateHdrFrame(LongRef out_frameId, int widthPx, int heightPx);
	public static native void phDeleteHdrFrame(long frameId);
}
