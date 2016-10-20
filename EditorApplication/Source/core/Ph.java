package core;

public final class Ph
{
	static
	{
		System.loadLibrary("./Library/JavaNativeInterface");
	}
	
	public static native void printTestMessage();
	public static native void genTestHdrFrame(FloatArrayRef out_pixelData, IntRef out_widthPx, IntRef out_heightPx);
}
