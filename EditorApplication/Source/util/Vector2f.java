package util;

public class Vector2f
{
	public float x;
	public float y;
	
	public Vector2f()
	{
		this(0.0f);
	}
	
	public Vector2f(float value)
	{
		this(0.0f, 0.0f);
	}
	
	public Vector2f(float x, float y)
	{
		this.x = x;
		this.y = y;
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
        
        final Vector2f other = (Vector2f)obj;
		
		return x == other.x && 
			   y == other.y;
	}

	@Override
	public int hashCode()
	{
		int result = 7;

		result = 37 * result + Float.floatToIntBits(x);
		result = 37 * result + Float.floatToIntBits(y);

		return result;
	}
	
	@Override
	public String toString()
	{
		return "(" + x + ", " + y + ")";
	}
}
