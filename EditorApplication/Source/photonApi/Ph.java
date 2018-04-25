package photonApi;

import java.nio.FloatBuffer;

public final class Ph
{
	// Notice: Photon-v2 library functions should not be called from threads other than the main thread.
	
	static
	{
		//System.loadLibrary("Release/Engine");
		System.loadLibrary("JNI");
	}
	
	// core
	
	public static native boolean phInit();
	public static native boolean phExit();
	
	public static native void phCreateEngine(LongRef out_engineId, int numRenderThreads);
	public static native void phEnterCommand(long engineId, String commandFragment);
	public static native void phUpdate(long engindId);
	public static native void phRender(long engineId);
	public static native void phDevelopFilm(long engineId, long frameId);
	public static native void phGetFilmDimension(long engineId, IntRef out_widthPx, IntRef out_heightPx);
	public static native void phDeleteEngine(long engineId);
	public static native void phSetWorkingDirectory(long engineId, String workingDirectory);
	
	public static native void phCreateFrame(LongRef out_frameId, int widthPx, int heightPx);
	public static native void phCopyFrameRgbData(long frameId, FloatArrayRef out_rgbData);
	public static native void phCopyFrameRgbData(long frameId, int xPx, int yPx, int wPx, int hPx, 
	                                             FloatArrayRef out_rgbData);
	public static native void phDeleteFrame(long frameId);
	
	// asynchronous operations
	
	public static final int FILM_REGION_STATUS_INVALID  = 0;
	public static final int FILM_REGION_STATUS_UPDATING = 1;
	public static final int FILM_REGION_STATUS_FINISHED = 2;
	
	public static native void phAsyncGetRendererStatistics(long engineId, 
	                                                       FloatRef out_percentageProgress, 
	                                                       FloatRef out_samplesPerSecond);
	
	public static native int  phAsyncPollUpdatedFilmRegion(long engineId, 
	                                                       IntRef out_xPx, IntRef out_yPx, 
	                                                       IntRef out_wPx, IntRef out_hPx);
	
	public static native void phAsyncDevelopFilmRegion(long engineId, long frameId, 
	                                                   int xPx, int yPx, int wPx, int hPx);
}
