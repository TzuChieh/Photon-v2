package util;

public class AABB2D
{
	public Vector2f min;
	public Vector2f max;
	
	public AABB2D()
	{
		this(new Vector2f(0), new Vector2f(0));
	}
	
	public AABB2D(Vector2f min, Vector2f max)
	{
		this.min = min;
		this.max = max;
	}
	
	public float getWidth()
	{
		return max.x - min.x;
	}
	
	public float getHeight()
	{
		return max.y - min.y;
	}
	
	public AABB2D expand(float amount)
	{
		return new AABB2D(min.sub(amount), max.add(amount));
	}
	
	public AABB2D floor()
	{
		return new AABB2D(min.floor(), max.floor());
	}
}
