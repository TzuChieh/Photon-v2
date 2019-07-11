package plugin.minecraft.nbt;

import java.io.IOException;
import java.io.InputStream;

public class CompoundTag extends NBTTag
{
	private NBTData m_data;
	
	public CompoundTag()
	{
		m_data = null;
	}
	
	@Override
	@SuppressWarnings("unchecked")
	public NBTData getPayload()
	{
		return m_data;
	}
	
	@Override
	public int setPayload(InputStream rawData) throws IOException
	{
		m_data = new NBTData();
		
		int numPayloadBytes = 0;
		while(true)
		{
			int id = rawData.read();
			if(id == -1)
			{
				break;
			}
			
			// an end tag, marks the end of compound tag
			if(id == 0)
			{
				NBTTag tag = new EndTag().fromRawData(rawData);
				numPayloadBytes += tag.numTagBytes();
				break;
			}
			
			NBTTag tag = null;
			switch(id)
			{
			case 1:  tag = new ByteTag();      break;
			case 2:  tag = new ShortTag();     break;
			case 3:  tag = new IntTag();       break;
			case 4:  tag = new LongTag();      break;
			case 5:  tag = new FloatTag();     break;
			case 6:  tag = new DoubleTag();    break;
			case 7:  tag = new ByteArrayTag(); break;
			case 8:  tag = new StringTag();    break;
			case 9:  tag = new ListTag();      break;
			case 10: tag = new CompoundTag();  break;
			case 11: tag = new IntArrayTag();  break;
			case 12: tag = new LongArrayTag(); break;
			
			default:
				System.err.println("warning: unknown id <" + id + ">, ignoring");
				break;
			}
			
			if(tag != null)
			{
				tag.fromRawData(rawData);
				numPayloadBytes += tag.numTagBytes();
				m_data.addTag(tag);
			}
		}
		
		return numPayloadBytes;
	}
	
	@Override
	public String toString()
	{
		return 
			"Compound Begin: " + getName() + "\n" + 
			m_data.toString() + 
			"Compound End: " + getName() + "\n";
	}
}
