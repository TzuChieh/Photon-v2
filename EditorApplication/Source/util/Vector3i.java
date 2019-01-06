package util;

public class Vector3i
{
	public int x;
	public int y;
	public int z;
	
	public Vector3i()
	{
		this(0, 0, 0);
	}
	
	public Vector3i(int x, int y, int z)
	{
		this.x = x;
		this.y = y;
		this.z = z;
	}
	
	public Vector3i add(Vector3i rhs)
	{
		return new Vector3i(x + rhs.x, y + rhs.y, z + rhs.z);
	}
	
	public Vector3i add(int rhs)
	{
		return new Vector3i(x + rhs, y + rhs, z + rhs);
	}
	
	public Vector3f toVector3f()
	{
		return new Vector3f(x, y, z);
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
        
        final Vector3i other = (Vector3i)obj;
		
		return x == other.x && 
			   y == other.y &&
			   z == other.z;
	}

	@Override
	public int hashCode()
	{
		int result = 7;

		result = 37 * result + x;
		result = 37 * result + y;
		result = 37 * result + z;

		return result;
	}
	
	@Override
	public String toString()
	{
		return "(" + x + ", " + y + ", " + z + ")";
	}
}
