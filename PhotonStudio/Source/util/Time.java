package util;

public final class Time
{
	public static double getTimeS()
	{
		return (double)System.nanoTime() / 1000000000.0;
	}
	
	public static double getTimeMs()
	{
		return (double)System.currentTimeMillis();
	}
	
	public static double getTimeUs()
	{
		return (double)System.nanoTime() / 1000.0;
	}
	
	public static double getTimeNs()
	{
		return (double)System.nanoTime();
	}
}
