package photonApi;

public final class Ph
{
	static
	{
		System.loadLibrary("Engine/Lib/Engine");
		System.loadLibrary("JNI/JNI");
	}
	
	public static native boolean init();
	
	public static native void printTestMessage();
	public static native void genTestHdrFrame(FloatArrayRef out_pixelData, IntRef out_widthPx, IntRef out_heightPx);
	
	// frame
	public static native void createHdrFrame(LongRef out_frameId, int widthPx, int heightPx);
	public static native void deleteHdrFrame(long frameId);
}
