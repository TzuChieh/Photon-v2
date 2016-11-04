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
	
	public static native boolean start();
	public static native void exit();
	
	// some tests
	
	public static native void printTestMessage();
	public static native void genTestHdrFrame(FloatArrayRef out_pixelData, IntRef out_widthPx, IntRef out_heightPx);
	
	// image related
	
	public static native void createHdrFrame(LongRef out_frameId, int widthPx, int heightPx);
	public static native void deleteHdrFrame(long frameId);
}
