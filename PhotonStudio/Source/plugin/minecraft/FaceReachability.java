package plugin.minecraft;

public class FaceReachability
{
	private short m_reachabilityBits;
	
	public FaceReachability()
	{
		m_reachabilityBits = 0;
	}
	
	public boolean isReachable(EFacing enter, EFacing exit)
	{
		final int bitIndex = getBitIndex(enter, exit);
		final int bit      = (m_reachabilityBits >> bitIndex) & 1;
		return bit == 1 ? true : false;
	}
	
	public boolean isFullyReachable()
	{
		return m_reachabilityBits == 0b0111_1111_1111_1111;
	}
	
	public boolean isFullyUnreachable()
	{
		return m_reachabilityBits == 0;
	}
	
	public boolean isReachable(EFacing from)
	{
		for(int f = 0; f < EFacing.SIZE; ++f)
		{
			if(f != from.getValue() && isReachable(from, EFacing.fromValue(f)))
			{
				return true;
			}
		}
		
		return false;
	}
	
	public void setReachability(EFacing enter, EFacing exit, boolean isReachable)
	{
		final int bitIndex = getBitIndex(enter, exit);
		if(isReachable)
		{
			m_reachabilityBits |= (short)(1 << bitIndex);
		}
		else
		{
			m_reachabilityBits &= (short)(~(1 << bitIndex) & 0b0111_1111_1111_1111);
		}
	}
	
	public void makeUnreachable(EFacing target)
	{
		for(int f = 0; f < EFacing.SIZE; ++f)
		{
			if(f != target.getValue())
			{
				setReachability(target, EFacing.fromValue(f), false);
			}
		}
	}
	
	public void makeFullyReachable()
	{
		m_reachabilityBits = 0b0111_1111_1111_1111;
	}
	
	public void addReachables(FaceReachability reachables)
	{
		m_reachabilityBits |= reachables.m_reachabilityBits;
	}
	
	private int getBitIndex(EFacing enter, EFacing exit)
	{
		assert(enter != exit);
		
		int faceA = enter.getValue();
		int faceB = exit.getValue();
		if(faceB < faceA)
		{
			faceA = exit.getValue();
			faceB = enter.getValue();
		}
		assert(faceA < faceB);
		
		// TODO: investigate faster methods, such as a table
		int bitIndex = Integer.MAX_VALUE;
		switch(faceA)
		{
		case 0: bitIndex = (faceB - 1);      break;
		case 1: bitIndex = 5 + (faceB - 2);  break;
		case 2: bitIndex = 9 + (faceB - 3);  break;
		case 3: bitIndex = 12 + (faceB - 4); break;
		case 4: bitIndex = 14;               break;
		}
		assert(bitIndex <= 14);
		
		return bitIndex;
	}
	
	@Override
	public String toString()
	{
		String binary = String.format("%15s", Integer.toBinaryString(m_reachabilityBits));
		binary = binary.replace(' ', '0');
		return "Face Reachability: " + binary;
	}
}
