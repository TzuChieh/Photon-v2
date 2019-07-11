package plugin.minecraft;

public class RegionCoord
{
	public int x;
	public int z;
	
	public RegionCoord()
	{
		this(0, 0);
	}
	
	public RegionCoord(int x, int z)
	{
		this.x = x;
		this.z = z;
	}
	
	@Override
	public boolean equals(Object obj)
	{
		if(obj == this)
            return true;
		
        if(obj == null)
            return false;
        
        if(getClass() != obj.getClass())
            return false;
        
        final RegionCoord other = (RegionCoord)obj;
		
		return x == other.x && 
		       z == other.z;
	}

	@Override
	public int hashCode()
	{
		int result = 7;

		result = 37 * result + x;
		result = 37 * result + z;

		return result;
	}
	
	@Override
	public String toString()
	{
		return "(" + x + ", " + z + ")"; 
	}
}
