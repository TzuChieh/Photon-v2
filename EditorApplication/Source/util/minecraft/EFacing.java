package util.minecraft;

public enum EFacing
{
	NORTH(0),
	EAST (1),
	SOUTH(2),
	WEST (3),
	UP   (4),
	DOWN (5);
	
	private final int m_value;
	
    private EFacing(int value)
    {
    	m_value = value;
    }

    public int getValue()
    {
        return m_value;
    }
}
