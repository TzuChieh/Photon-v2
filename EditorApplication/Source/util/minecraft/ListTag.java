package util.minecraft;

import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.stream.Collectors;

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
			numPayloadBytes += 0;
			
			m_list = Arrays.asList(new Void[size]);
		}
		break;
		
		case 1:
		{
			numPayloadBytes += size;
			
			List<Byte> bytes = new ArrayList<>();
			for(byte value : readArray(size, rawData))
			{
				bytes.add(value);
			}
			m_list = bytes;
		}
		break;
		
		case 2:
		{
			numPayloadBytes += size * Short.BYTES;
			
			List<Short> shorts = new ArrayList<>();
			for(short value : readShortArray(size, rawData))
			{
				shorts.add(value);
			}
			m_list = shorts;
			
		}
		break;
		
		case 3:
		{
			numPayloadBytes += size * Integer.BYTES;
			
			int[] integers = readIntArray(size, rawData);
			m_list = Arrays.stream(integers).boxed().collect(Collectors.toList());
		}
		break;	
		
		case 4:
		{
			numPayloadBytes += size * Long.BYTES;
			
			long[] longs = readLongArray(size, rawData);
			m_list = Arrays.stream(longs).boxed().collect(Collectors.toList());
		}
		break;
		
		case 5:
		{
			numPayloadBytes += size * Float.BYTES;
			
			List<Float> floats = new ArrayList<>();
			for(float value : readFloatArray(size, rawData))
			{
				floats.add(value);
			}
			m_list = floats;
		}
		break;
		
		case 6:
		{
			numPayloadBytes += size * Double.BYTES;
			
			List<Double> doubles = new ArrayList<>();
			for(double value : readDoubleArray(size, rawData))
			{
				doubles.add(value);
			}
			m_list = doubles;
		}
		break;
		
		case 7:
		{
			List<byte[]> list = new ArrayList<>();
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
			List<String> list = new ArrayList<>();
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
			List<List<?>> list = new ArrayList<>();
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
			List<NBTData> list = new ArrayList<>();
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
			List<int[]> list = new ArrayList<>();
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
			List<long[]> list = new ArrayList<>();
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
	
	@Override
	public String toString()
	{
		String result = "List Begin: " + getName() + "\n";
		for(int i = 0; i < m_list.size(); ++i)
		{
			result += i + ": " + m_list.get(i).toString() + "\n";
		}
		result += "List End: " + getName() + "\n";
		
		return result;
	}
}
