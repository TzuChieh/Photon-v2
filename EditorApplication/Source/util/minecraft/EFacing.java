package util.minecraft;

public enum EFacing
{
	NORTH(0),
	EAST (1),
	SOUTH(2),
	WEST (3),
	UP   (4),
	DOWN (5);
	
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
	    
	public static EFacing fromValue(int value)
	{
		switch(value)
		{
		case 0: return NORTH;
		case 1: return EAST;
		case 2: return SOUTH;
		case 3: return WEST;
		case 4: return UP;
		case 5: return DOWN;
		}
		
		return null;
	}
}
