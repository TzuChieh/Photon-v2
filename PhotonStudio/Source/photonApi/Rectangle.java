package photonApi;

public final class Rectangle
{
	public int x;
	public int y;
	public int w;
	public int h;
	
	public Rectangle(int x, int y, int w, int h)
	{
		this.x = x;
		this.y = y;
		this.w = w;
		this.h = h;
	}
	
	public Rectangle()
	{
		this(0, 0, 0, 0);
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
        
        final Rectangle other = (Rectangle)obj;
		
		return x == other.x && 
		       y == other.y &&
		       w == other.w &&
		       h == other.h;
	}
	
	@Override
	public int hashCode()
	{
		int result = 7;

		result = 37 * result + x;
		result = 37 * result + y;
		result = 37 * result + w;
		result = 37 * result + h;
		
		return result;
	}
	
	@Override
	public String toString()
	{
		return "x = " + x + ", y = " + y + ", w = " + w + ", h = " + h;
	}
}
