package photonApi;

public final class Ph
{
	// Notice: Photon-v2 library functions should not be called from threads other than the main thread.
	
	static
	{
		System.loadLibrary("Engine/Lib/Engine");
		System.loadLibrary("JNI/JNI");
	}
	
	public static final int PH_PREVIEW_RENDERER_TYPE              = 1;
	public static final int PH_IMPORTANCE_RENDERER_TYPE           = 2;
	public static final int PH_MT_IMPORTANCE_RENDERER_TYPE        = 3;
	public static final int PH_HDR_FRAME_TYPE                     = 50;
	public static final int PH_DEFAULT_CAMERA_TYPE                = 100;
	public static final int PH_PIXEL_JITTER_SAMPLE_GENERATOR_TYPE = 150;
	
	// core
	
	public static native boolean phStart();
	public static native void    phExit();
	
	public static native void    phCreateRenderer(LongRef out_rendererId, int rendererType, int numThreads);
	public static native void    phDeleteRenderer(long rendererId);
	
	public static native void    phCreateFilm(LongRef out_filmId, int filmWidthPx, int filmHeightPx);
	public static native void    phDeleteFilm(long filmId);
	
	public static native void    phCreateCamera(LongRef out_cameraId, int cameraType);
	public static native void    phDeleteCamera(long cameraId);
	
	public static native void    phCreateFrame(LongRef out_frameId, int frameType, int widthPx, int heightPx);
	public static native void    phDeleteFrame(long frameId);
	
	public static native void    phCreateWorld(LongRef out_worldId);
	public static native void    phDeleteWorld(long worldId);
	
	public static native void    phCreateSampleGenerator(LongRef out_sampleGeneratorId, int sampleGeneratorType, int sppBudget);
	public static native void    phDeleteSampleGenerator(long sampleGeneratorId);
	
	public static native void    phRender(long rendererId, long worldId, long cameraId);
	public static native void    phLoadTestScene(long worldId);
	public static native void    phSetCameraFilm(long cameraId, long filmId);
	public static native void    phSetRendererSampleGenerator(long rendererId, long sampleGeneratorId);
	public static native void    phCookWorld(long worldId);
	public static native void    phDevelopFilm(long filmId, long frameId);
	public static native void    phGetFrameData(long frameId, FloatArrayRef out_pixelData, IntRef out_widthPx, IntRef out_heightPx, IntRef out_nPixelComponents);
	
	public static native void    phSetCameraPosition(long cameraId, float x, float y, float z);
	public static native void    phSetCameraDirection(long cameraId, float x, float y, float z);
	
	// some tests
	
	public static native void printTestMessage();
	public static native void genTestHdrFrame(FloatArrayRef out_pixelData, IntRef out_widthPx, IntRef out_heightPx);
	
	// query
	
	public static native void phQueryRendererPercentageProgress(long rendererId, FloatRef out_progress);
	public static native void phQueryRendererSampleFrequency(long rendererId, FloatRef out_frequency);
}
