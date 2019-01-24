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
	
	public Vector2f add(Vector2f rhs)
	{
		return new Vector2f(x + rhs.x, y + rhs.y);
	}
	
	public Vector2f add(float rhs)
	{
		return new Vector2f(x + rhs, y + rhs);
	}
	
	public Vector2f sub(Vector2f rhs)
	{
		return new Vector2f(x - rhs.x, y - rhs.y);
	}
	
	public Vector2f sub(float rhs)
	{
		return new Vector2f(x - rhs, y - rhs);
	}
	
	public Vector2f mul(Vector2f rhs)
	{
		return new Vector2f(x * rhs.x, y * rhs.y);
	}
	
	public Vector2f mul(float rhs)
	{
		return new Vector2f(x * rhs, y * rhs);
	}
	
	public Vector2f div(Vector2f rhs)
	{
		return new Vector2f(x / rhs.x, y / rhs.y);
	}
	
	public Vector2f div(float rhs)
	{
		return new Vector2f(x / rhs, y / rhs);
	}
	
	public Vector2f floor()
	{
		return new Vector2f((float)Math.floor(x), (float)Math.floor(y));
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
