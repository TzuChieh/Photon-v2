package minecraft;

public enum EFacing
{
	NORTH(0),
	SOUTH(1),
	WEST (2),
	EAST (3),
	DOWN (4),
	UP   (5);
	
	public static final int SIZE = 6;
	
	private final int m_value;
	
	private EFacing(int value)
	{
		m_value = value;
	}
	
	public int getValue()
	{
		return m_value;
	}
	
	public EFacing getOpposite()
	{
		switch(m_value)
		{
		case 0: return SOUTH;
		case 1: return NORTH;
		case 2: return EAST;
		case 3: return WEST;
		case 4: return UP;
		case 5: return DOWN;
		}
		
		return null;
	}
	    
	public static EFacing fromValue(int value)
	{
		switch(value)
		{
		case 0: return NORTH;
		case 1: return SOUTH;
		case 2: return WEST;
		case 3: return EAST;
		case 4: return DOWN;
		case 5: return UP;
		}
		
		return null;
	}
}
