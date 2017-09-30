package photonApi;

import util.Time;

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
}
