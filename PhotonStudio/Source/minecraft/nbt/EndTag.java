package minecraft.nbt;

import java.io.IOException;
import java.io.InputStream;

public class EndTag extends NBTTag
{
	@Override
	@SuppressWarnings("unchecked")
	public Void getPayload()
	{
		return null;
	}
	
	@Override
	public int setPayload(InputStream rawData) throws IOException
	{
		// no payload to set
		
		return 0;
	}
}
