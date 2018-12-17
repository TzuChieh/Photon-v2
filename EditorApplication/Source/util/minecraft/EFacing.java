package util.minecraft;

public enum EFacing
{
	NORTH(0),
	EAST (1),
	SOUTH(2),
	WEST (3),
	UP   (4),
	DOWN (5),
	
	NEGATIVE_X(WEST.getValue()),
	POSITIVE_X(EAST.getValue()),
	NEGATIVE_Y(DOWN.getValue()),
	POSITIVE_Y(UP.getValue()),
	NEGATIVE_Z(NORTH.getValue()),
	POSITIVE_Z(SOUTH.getValue());
	
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
