package photonApi;

public final class Ph
{
	// Notice: Photon-v2 library functions should not be called from threads other than the main thread.
	
	static
	{
		System.loadLibrary("Engine/Lib/Engine");
		System.loadLibrary("JNI/JNI");
	}
	
	public static final int PH_BRUTE_FORCE_RENDERER_TYPE = 1;
	public static final int PH_IMPORTANCE_RENDERER_TYPE  = 2;
	public static final int PH_HDR_FRAME_TYPE            = 50;
	public static final int PH_DEFAULT_CAMERA_TYPE       = 100;
	
	// core
	
	public static native boolean phStart();
	public static native void    phExit();
	public static native void    phCreateRenderer(LongRef out_rendererId, int rendererType);
	public static native void    phDeleteRenderer(long rendererId);
	public static native void    phCreateCamera(LongRef out_cameraId, int cameraType, int filmWidthPx, int filmHeightPx);
	public static native void    phDeleteCamera(long cameraId);
	public static native void    phCreateWorld(LongRef out_worldId);
	public static native void    phDeleteWorld(long worldId);
	public static native void    phCreateRenderTask(LongRef out_renderTaskId, long worldId, long cameraId, long rendererId, long frameId);
	public static native void    phDeleteRenderTask(long renderTaskId);
	public static native void    phRunRenderTask(long renderTaskId);
	
	// some tests
	
	public static native void printTestMessage();
	public static native void genTestHdrFrame(FloatArrayRef out_pixelData, IntRef out_widthPx, IntRef out_heightPx);
	
	// image related
	
	public static native void phCreateFrame(LongRef out_frameId, int widthPx, int heightPx, int frameType);
	public static native void phDeleteFrame(long frameId);
}
