package util.minecraft;

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

public abstract class NBTTag
{
	private String m_name;
	private int    m_numTagBytes;
	private int    m_numPayloadBytes;
	
	protected NBTTag()
	{
		m_name            = "";
		m_numTagBytes     = 0;
		m_numPayloadBytes = 0;
	}
	
	public abstract <T> T getPayload();
	public abstract int setPayload(InputStream rawData) throws IOException;
	
	public NBTTag fromRawData(InputStream rawData) throws IOException
	{
		final int NUM_ID_BYTES          = 1;
		final int NUM_NAME_LENGTH_BYTES = 2;
		
		if(this instanceof EndTag)
		{
			setName("");
			m_numPayloadBytes = setPayload(rawData);
			m_numTagBytes = NUM_ID_BYTES + m_numPayloadBytes;
			return this;
		}
		
		int numNameBytes = ((rawData.read() << 8) | rawData.read());
		
		String tagName = readStringUTF8(numNameBytes, rawData);
		setName(tagName);
		
		m_numPayloadBytes = setPayload(rawData);
		m_numTagBytes = 
			NUM_ID_BYTES + 
			NUM_NAME_LENGTH_BYTES + 
			numNameBytes + 
			m_numPayloadBytes;
		
		return this;
	}
	
	public String getName()
	{
		return m_name;
	}
	
	public NBTTag setName(String name)
	{
		m_name = name;
		
		return this;
	}
	
	public int numTagBytes()
	{
		return m_numTagBytes;
	}
	
	public int numPayloadBytes()
	{
		return m_numPayloadBytes;
	}
	
	public static byte[] readArray(int numBytes, InputStream rawData) throws IOException
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
	
	public static ByteBuffer readBuffer(int numBytes, InputStream rawData) throws IOException
	{
		byte[] array = readArray(numBytes, rawData);
		return ByteBuffer.wrap(array).order(ByteOrder.BIG_ENDIAN);
	}
	
	public static short[] readShortArray(int size, InputStream rawData) throws IOException
	{
		ShortBuffer shortBuffer = readBuffer(size * Short.BYTES, rawData).asShortBuffer();
		
		short[] array = new short[shortBuffer.remaining()];
		shortBuffer.get(array);
		return array;
	}
	
	public static int[] readIntArray(int size, InputStream rawData) throws IOException
	{
		IntBuffer intBuffer = readBuffer(size * Integer.BYTES, rawData).asIntBuffer();
		
		int[] array = new int[intBuffer.remaining()];
		intBuffer.get(array);
		return array;
	}
	
	public static long[] readLongArray(int size, InputStream rawData) throws IOException
	{
		LongBuffer longBuffer = readBuffer(size * Long.BYTES, rawData).asLongBuffer();
		
		long[] array = new long[longBuffer.remaining()];
		longBuffer.get(array);
		return array;
	}
	
	public static float[] readFloatArray(int size, InputStream rawData) throws IOException
	{
		FloatBuffer floatBuffer = readBuffer(size * Float.BYTES, rawData).asFloatBuffer();
		
		float[] array = new float[floatBuffer.remaining()];
		floatBuffer.get(array);
		return array;
	}
	
	public static double[] readDoubleArray(int size, InputStream rawData) throws IOException
	{
		DoubleBuffer doubleBuffer = readBuffer(size * Double.BYTES, rawData).asDoubleBuffer();
		
		double[] array = new double[doubleBuffer.remaining()];
		doubleBuffer.get(array);
		return array;
	}
	
	public static String readStringUTF8(int numBytes, InputStream rawData) throws IOException
	{
		byte[] stringBytes = readArray(numBytes, rawData);
		return new String(stringBytes, StandardCharsets.UTF_8);
	}
}
