package util.minecraft;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.IntBuffer;
import java.nio.LongBuffer;
import java.nio.charset.StandardCharsets;

public class NBTParser
{
	private NBTData m_data;
	
	public NBTParser(InputStream rawData)
	{
		m_data = null;
	}
	
	public NBTData getData()
	{
		return m_data;
	}
	
	private NBTData parse(InputStream rawData) throws IOException
	{
		NBTData data = new NBTData();
		
		while(true)
		{
			int id = rawData.read();
			if(id == -1)
			{
				break;
			}
			
			if(id == 0)
			{
				data.add(new EndTag());
				continue;
			}
			
			int numNameBytes = ((rawData.read() << 8) | rawData.read());
			byte[] nameBytes = new byte[numNameBytes];
			rawData.read(nameBytes);
			
			NBTTag tagData = null;
			switch(id)
			{
			case 1:
			{
				ByteTag tag = new ByteTag();
				tag.setValue((byte)rawData.read());
				tagData = tag;
			}
			break;
				
			case 2:
			{
				ShortTag tag = new ShortTag();
				tag.setValue((short)((rawData.read() << 8) | rawData.read()));
				tagData = tag;
			}
			break;
			
			case 3:
			{
				IntTag tag = new IntTag();
				int value = 
					(rawData.read() << 24) | 
					(rawData.read() << 16) | 
					(rawData.read() << 8 ) | 
					(rawData.read());
				tag.setValue(value);
				tagData = tag;
			}
			break;
			
			case 4:
			{
				LongTag tag = new LongTag();
				long value = 
					((long)(rawData.read()) << 56) | 
					((long)(rawData.read()) << 48) | 
					((long)(rawData.read()) << 40) | 
					((long)(rawData.read()) << 32) | 
					((long)(rawData.read()) << 24) | 
					((long)(rawData.read()) << 16) | 
					((long)(rawData.read()) << 8 ) | 
					((long)(rawData.read()));
				tag.setValue(value);
				tagData = tag;
			}
			break;
			
			case 5:
			{
				FloatTag tag = new FloatTag();
				int bits = 
					(rawData.read() << 24) | 
					(rawData.read() << 16) | 
					(rawData.read() << 8 ) | 
					(rawData.read());
				tag.setValue(Float.intBitsToFloat(bits));
				tagData = tag;
			}
			break;
			
			case 6:
			{
				DoubleTag tag = new DoubleTag();
				long bits = 
					((long)(rawData.read()) << 56) | 
					((long)(rawData.read()) << 48) | 
					((long)(rawData.read()) << 40) | 
					((long)(rawData.read()) << 32) | 
					((long)(rawData.read()) << 24) | 
					((long)(rawData.read()) << 16) | 
					((long)(rawData.read()) << 8 ) | 
					((long)(rawData.read()));
				tag.setValue(Double.longBitsToDouble(bits));
				tagData = tag;
			}
			break;
			
			case 7:
			{
				ByteArrayTag tag = new ByteArrayTag();
				int size = 
					(rawData.read() << 24) | 
					(rawData.read() << 16) | 
					(rawData.read() << 8 ) | 
					(rawData.read());
				byte[] array = new byte[size];
				rawData.read(array);
				tag.setArray(array);
			}
			break;
			
			case 8:
			{
				StringTag tag = new StringTag();
				int numBytes = ((rawData.read() << 8) | rawData.read());
				byte[] stringBytes = new byte[numBytes];
				rawData.read(stringBytes);
				tag.setData(new String(nameBytes, StandardCharsets.UTF_8));
			}
			break;
			
			case 9:
			{
				int tagId = rawData.read();
				int size = 
						(rawData.read() << 24) | 
						(rawData.read() << 16) | 
						(rawData.read() << 8 ) | 
						(rawData.read());
				// TODO
			}
			break;
			
			case 10:
			{
				// TODO
			}
			break;
			
			case 11:
			{
				IntArrayTag tag = new IntArrayTag();
				int size = 
					(rawData.read() << 24) | 
					(rawData.read() << 16) | 
					(rawData.read() << 8 ) | 
					(rawData.read());
				byte[] buffer = new byte[size * 4];
				rawData.read(buffer);
				
				IntBuffer intBuffer = 
					ByteBuffer.wrap(buffer).
					order(ByteOrder.BIG_ENDIAN).
					asIntBuffer();
				
				int[] array = new int[intBuffer.remaining()];
				intBuffer.get(array);
				tag.setArray(array);
			}
			break;

			case 12:
			{
				LongArrayTag tag = new LongArrayTag();
				int size = 
					(rawData.read() << 24) | 
					(rawData.read() << 16) | 
					(rawData.read() << 8 ) | 
					(rawData.read());
				byte[] buffer = new byte[size * 8];
				rawData.read(buffer);
				
				LongBuffer longBuffer = 
					ByteBuffer.wrap(buffer).
					order(ByteOrder.BIG_ENDIAN).
					asLongBuffer();
				
				long[] array = new long[longBuffer.remaining()];
				longBuffer.get(array);
				tag.setArray(array);
			}
			break;
			
			default:
				System.err.println("warning: unknown id <" + id + ">, ignoring");
				break;
			}
			
			if(tagData != null)
			{
				String tagName = new String(nameBytes, StandardCharsets.UTF_8);
				tagData.setName(tagName);
				data.add(tagData);
			}
		}
		
		return data;
	}
}
