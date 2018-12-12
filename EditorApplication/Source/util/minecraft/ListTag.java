package util.minecraft;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.DoubleBuffer;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class ListTag extends NBTTag
{
	private List<?> m_list;
	
	public ListTag()
	{
		m_list = null;
	}
	
	@Override
	@SuppressWarnings("unchecked")
	public <T> T getPayload()
	{
		return (T)m_list;
	}
	
	@Override
	public int setPayload(InputStream rawData) throws IOException
	{
		int tagId = rawData.read();
		int size = 
			(rawData.read() << 24) | 
			(rawData.read() << 16) | 
			(rawData.read() << 8 ) | 
			(rawData.read());
		
		int numPayloadBytes = 1 + 4;
		switch(tagId)
		{
		case 0:
		{
			m_list = Arrays.asList(new Void[size]);
			numPayloadBytes += 0;
		}
		break;
		
		case 1:
		{
			ByteArrayTag tag = new ByteArrayTag();
			numPayloadBytes += tag.setPayload(rawData);
			m_list = Arrays.asList(tag.getPayload());
		}
		break;
		
		case 2:
		{
			int numBufferBytes = size * Short.BYTES;
			numPayloadBytes += numBufferBytes;
			byte[] buffer = new byte[numBufferBytes];
			rawData.read(buffer);
			
			ShortBuffer shortBuffer = 
				ByteBuffer.wrap(buffer).
				order(ByteOrder.BIG_ENDIAN).
				asShortBuffer();
			
			short[] array = new short[shortBuffer.remaining()];
			shortBuffer.get(array);
			
			m_list = Arrays.asList(array);
		}
		break;
		
		case 3:
		{
			IntArrayTag tag = new IntArrayTag();
			numPayloadBytes += tag.setPayload(rawData);
			m_list = Arrays.asList(tag.getPayload());
		}
		break;	
		
		case 4:
		{
			LongArrayTag tag = new LongArrayTag();
			numPayloadBytes += tag.setPayload(rawData);
			m_list = Arrays.asList(tag.getPayload());
		}
		break;
		
		case 5:
		{
			int numBufferBytes = size * Float.BYTES;
			numPayloadBytes += numBufferBytes;
			byte[] buffer = new byte[numBufferBytes];
			rawData.read(buffer);
			
			FloatBuffer floatBuffer = 
				ByteBuffer.wrap(buffer).
				order(ByteOrder.BIG_ENDIAN).
				asFloatBuffer();
			
			float[] array = new float[floatBuffer.remaining()];
			floatBuffer.get(array);
			
			m_list = Arrays.asList(array);
		}
		break;
		
		case 6:
		{
			int numBufferBytes = size * Double.BYTES;
			numPayloadBytes += numBufferBytes;
			byte[] buffer = new byte[numBufferBytes];
			rawData.read(buffer);
			
			DoubleBuffer doubleBuffer = 
				ByteBuffer.wrap(buffer).
				order(ByteOrder.BIG_ENDIAN).
				asDoubleBuffer();
			
			double[] array = new double[doubleBuffer.remaining()];
			doubleBuffer.get(array);
			
			m_list = Arrays.asList(array);
		}
		break;
		
		case 7:
		{
			ArrayList<byte[]> list = new ArrayList<>();
			for(int i = 0; i < size; ++i)
			{
				ByteArrayTag tag = new ByteArrayTag();
				numPayloadBytes += tag.setPayload(rawData);
				list.add(tag.getPayload());
			}
			m_list = list;
		}
		break;
		
		case 8:
		{
			ArrayList<String> list = new ArrayList<>();
			for(int i = 0; i < size; ++i)
			{
				StringTag tag = new StringTag();
				numPayloadBytes += tag.setPayload(rawData);
				list.add(tag.getPayload());
			}
			m_list = list;
		}
		break;
		
		case 9:
		{
			ArrayList<List<?>> list = new ArrayList<>();
			for(int i = 0; i < size; ++i)
			{
				ListTag tag = new ListTag();
				numPayloadBytes += tag.setPayload(rawData);
				list.add(tag.getPayload());
			}
			m_list = list;
		}
		break;
		
		case 10:
		{
			ArrayList<NBTData> list = new ArrayList<>();
			for(int i = 0; i < size; ++i)
			{
				CompoundTag tag = new CompoundTag();
				numPayloadBytes += tag.setPayload(rawData);
				list.add(tag.getPayload());
			}
			m_list = list;
		}
		break;
		
		case 11:
		{
			ArrayList<int[]> list = new ArrayList<>();
			for(int i = 0; i < size; ++i)
			{
				IntArrayTag tag = new IntArrayTag();
				numPayloadBytes += tag.setPayload(rawData);
				list.add(tag.getPayload());
			}
			m_list = list;
		}
		
		case 12:
		{
			ArrayList<long[]> list = new ArrayList<>();
			for(int i = 0; i < size; ++i)
			{
				LongArrayTag tag = new LongArrayTag();
				numPayloadBytes += tag.setPayload(rawData);
				list.add(tag.getPayload());
			}
			m_list = list;
		}
		
		default:
			System.err.println("warning: unknown id <" + tagId + ">, ignoring");
			break;
		}
		
		return numPayloadBytes;
	}
}
