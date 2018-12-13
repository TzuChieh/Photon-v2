package util;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.DoubleBuffer;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;
import java.nio.LongBuffer;
import java.nio.ShortBuffer;
import java.nio.charset.StandardCharsets;

public class BinaryData
{
	public static byte[] readByteArray(int numBytes, InputStream rawData) throws IOException
	{
		byte[] buffer = new byte[numBytes];
		int readBytes = 0;
		while(readBytes < numBytes)
		{
			int read = rawData.read(buffer, readBytes, numBytes - readBytes);
			if(read == -1)
			{
				throw new IOException(
					"expected " + numBytes + "available for reading, only " + readBytes + " present");
			}
			readBytes += read;
		}
		return buffer;
	}
	
	public static ByteBuffer readByteBuffer(int numBytes, InputStream rawData) throws IOException
	{
		byte[] array = readByteArray(numBytes, rawData);
		return ByteBuffer.wrap(array).order(ByteOrder.BIG_ENDIAN);
	}
	
	public static short[] readShortArray(int size, InputStream rawData) throws IOException
	{
		ShortBuffer shortBuffer = readByteBuffer(size * Short.BYTES, rawData).asShortBuffer();
		
		short[] array = new short[shortBuffer.remaining()];
		shortBuffer.get(array);
		return array;
	}
	
	public static int[] readIntArray(int size, InputStream rawData) throws IOException
	{
		IntBuffer intBuffer = readByteBuffer(size * Integer.BYTES, rawData).asIntBuffer();
		
		int[] array = new int[intBuffer.remaining()];
		intBuffer.get(array);
		return array;
	}
	
	public static long[] readLongArray(int size, InputStream rawData) throws IOException
	{
		LongBuffer longBuffer = readByteBuffer(size * Long.BYTES, rawData).asLongBuffer();
		
		long[] array = new long[longBuffer.remaining()];
		longBuffer.get(array);
		return array;
	}
	
	public static float[] readFloatArray(int size, InputStream rawData) throws IOException
	{
		FloatBuffer floatBuffer = readByteBuffer(size * Float.BYTES, rawData).asFloatBuffer();
		
		float[] array = new float[floatBuffer.remaining()];
		floatBuffer.get(array);
		return array;
	}
	
	public static double[] readDoubleArray(int size, InputStream rawData) throws IOException
	{
		DoubleBuffer doubleBuffer = readByteBuffer(size * Double.BYTES, rawData).asDoubleBuffer();
		
		double[] array = new double[doubleBuffer.remaining()];
		doubleBuffer.get(array);
		return array;
	}
	
	public static String readStringUTF8(int numBytes, InputStream rawData) throws IOException
	{
		byte[] stringBytes = readByteArray(numBytes, rawData);
		return new String(stringBytes, StandardCharsets.UTF_8);
	}
	
	public static short readShort(InputStream rawData) throws IOException
	{
		byte[] bytes = readByteArray(Short.BYTES, rawData);
		
		return (short)(((bytes[0] & 0xFF) << 8) | (bytes[1] & 0xFF));
	}
	
	public static int readInt(InputStream rawData) throws IOException
	{
		byte[] bytes = readByteArray(Integer.BYTES, rawData);
		
		return 
			((bytes[0] & 0xFF) << 24) | 
			((bytes[1] & 0xFF) << 16) | 
			((bytes[2] & 0xFF) << 8 ) | 
			((bytes[3] & 0xFF));
	}
	
	public static long readLong(InputStream rawData) throws IOException
	{
		byte[] bytes = readByteArray(Long.BYTES, rawData);
		
		return 
			((long)(bytes[0] & 0xFF) << 56) | 
			((long)(bytes[1] & 0xFF) << 48) | 
			((long)(bytes[2] & 0xFF) << 40) | 
			((long)(bytes[3] & 0xFF) << 32) | 
			((long)(bytes[4] & 0xFF) << 24) | 
			((long)(bytes[5] & 0xFF) << 16) | 
			((long)(bytes[6] & 0xFF) << 8 ) | 
			((long)(bytes[7] & 0xFF));
	}
	
	public static float readFloat(InputStream rawData) throws IOException
	{
		return Float.intBitsToFloat(readInt(rawData));
	}
	
	public static double readDouble(InputStream rawData) throws IOException
	{
		return Double.longBitsToDouble(readLong(rawData));
	}
	
	public static byte[] readAll(InputStream rawData) throws IOException
	{
		final int BUFFER_SIZE = 8192;
		
		ByteArrayOutputStream output = new ByteArrayOutputStream();
		
		byte[] buffer = new byte[BUFFER_SIZE];
		while(true)
		{
			int readBytes = rawData.read(buffer, 0, BUFFER_SIZE);
			if(readBytes != -1)
			{
				output.write(buffer, 0, readBytes);
			}
			else
			{
				break;
			}
		}
		
		return output.toByteArray();
	}
}
