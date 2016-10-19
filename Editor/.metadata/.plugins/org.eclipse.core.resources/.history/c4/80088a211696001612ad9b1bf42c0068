//	The MIT License (MIT)
//	
//	Copyright (c) 2016 Tzu-Chieh Chang (as known as D01phiN)
//	
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:
//	
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//	
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.

package math;

public class Vector3f
{
	public static final Vector3f VEC3_ONE_ONE_ONE     = new Vector3f(1, 1, 1);
	public static final Vector3f VEC3_ZERO_ZERO_ZERO  = new Vector3f(0, 0, 0);
	
	public static final Vector3f UNIT_X_AXIS          = new Vector3f(1, 0, 0);
	public static final Vector3f UNIT_Y_AXIS          = new Vector3f(0, 1, 0);
	public static final Vector3f UNIT_Z_AXIS          = new Vector3f(0, 0, 1);
	
	public static final Vector3f UNIT_NEGATIVE_X_AXIS = new Vector3f(-1, 0, 0);
	public static final Vector3f UNIT_NEGATIVE_Y_AXIS = new Vector3f(0, -1, 0);
	public static final Vector3f UNIT_NEGATIVE_Z_AXIS = new Vector3f(0, 0, -1);
	
	public float x;
	public float y;
	public float z;
	
	public Vector3f(Vector3f copyVector3f)
	{
		this.x = copyVector3f.x;
		this.y = copyVector3f.y;
		this.z = copyVector3f.z;
	}
	
	public Vector3f(float x, float y, float z)
	{
		this.x = x;
		this.y = y;
		this.z = z;
	}
	
	public Vector3f(float var)
	{
		this(var, var, var);
	}
	
	public Vector3f()
	{
		this.x = 0;
		this.y = 0;
		this.z = 0;
	}

	public float length()
	{
		return (float)Math.sqrt(x * x + y * y + z * z);
	}
	
	public float squareLength()
	{
		return x * x + y * y + z * z;
	}

	public float max()
	{
		return Math.max(x, Math.max(y, z));
	}
	
	public float absMax()
	{
		return Math.max(Math.abs(x), Math.max(Math.abs(y), Math.abs(z)));
	}
	
	public Vector3f max(Vector3f r)
	{
		return new Vector3f(Math.max(x, r.x), Math.max(y, r.y), Math.max(z, r.z));
	}
	
	public Vector3f maxLocal(Vector3f r)
	{
		x = Math.max(x, r.x);
		y = Math.max(y, r.y);
		z = Math.max(z, r.z);
		
		return this;
	}
	
	public Vector3f min(Vector3f r)
	{
		return new Vector3f(Math.min(x, r.x), Math.min(y, r.y), Math.min(z, r.z));
	}
	
	public Vector3f minLocal(Vector3f r)
	{
		x = Math.min(x, r.x);
		y = Math.min(y, r.y);
		z = Math.min(z, r.z);
		
		return this;
	}

	public float dot(Vector3f r)
	{
		return x * r.x + y * r.y + z * r.z;
	}
	
	public float absDot(Vector3f r)
	{
		return Math.abs(dot(r));
	}
	
	public float dot(float var)
	{
		return x * var + y * var + z * var;
	}
	
	public float dot(float varX, float varY, float varZ)
	{
		return x * varX + y * varY + z * varZ;
	}
	
	public Vector3f cross(Vector3f r)
	{
		float _x = y * r.z - z * r.y;
		float _y = z * r.x - x * r.z;
		float _z = x * r.y - y * r.x;
		
		return new Vector3f(_x, _y, _z);
	}
	
	public void cross(Vector3f value, Vector3f result)
	{
		result.x = y * value.z - z * value.y;
		result.y = z * value.x - x * value.z;
		result.z = x * value.y - y * value.x;
	}
	
	public Vector3f normalize()
	{
		float length = length();
	
		return new Vector3f(x / length, y / length, z / length);
	}
	
	public Vector3f setLength(float length)
	{
		float multiplier = length / length();
		
		x *= multiplier;
		y *= multiplier;
		z *= multiplier;
		
		return this;
	}
	
	public Vector3f normalizeLocal()
	{
		float length = length();
		
		x /= length;
		y /= length;
		z /= length;
	
		return this;
	}

	/****************************************************/
	// rotate
	public Vector3f rotate(Quaternion rotation)
	{
		rotation.conjugate(Quaternion.TEMP.get());

		Quaternion w = rotation.mul(this).mulLocal(Quaternion.TEMP.get());

		return new Vector3f(w.getX(), w.getY(), w.getZ());
	}

	public Vector3f lerp(Vector3f destination, float lerpFactor)
	{
		return destination.sub(this).mulLocal(lerpFactor).addLocal(this);
	}
	
	public Vector3f setLinearInterpolated(Vector3f start, Vector3f end, float fraction)
	{
		this.x = (end.x - start.x) * fraction + start.x;
		this.y = (end.y - start.y) * fraction + start.y;
		this.z = (end.z - start.z) * fraction + start.z;
		
		return this;
	}
	
	/****************************************************/
	// add
	public Vector3f add(Vector3f var)
	{
		return new Vector3f(x + var.x, y + var.y, z + var.z);
	}
	
	public Vector3f add(Vector3f var, Vector3f result)
	{
		result.x = x + var.x;
		result.y = y + var.y;
		result.z = z + var.z;
		
		return result;
	}
	
	public Vector3f add(float r)
	{
		return new Vector3f(x + r, y + r, z + r);
	}
	
	public Vector3f addLocal(Vector3f var)
	{
		x += var.x;
		y += var.y;
		z += var.z;
		
		return this;
	}
	
	public Vector3f addLocal(float var)
	{
		x += var;
		y += var;
		z += var;
		
		return this;
	}
	
	public Vector3f addLocal(float varX, float varY, float varZ)
	{
		x += varX;
		y += varY;
		z += varZ;
		
		return this;
	}

	/****************************************************/
	// sub
	public Vector3f sub(Vector3f r)
	{
		return new Vector3f(x - r.getX(), y - r.getY(), z - r.getZ());
	}
	
	public Vector3f sub(float r)
	{
		return new Vector3f(x - r, y - r, z - r);
	}
	
	public Vector3f sub(Vector3f var, Vector3f result)
	{
		result.x = x - var.x;
		result.y = y - var.y;
		result.z = z - var.z;
		
		return result;
	}
	
	public Vector3f subLocal(Vector3f var)
	{
		this.x -= var.x;
		this.y -= var.y;
		this.z -= var.z;
		
		return this;
	}
	
	public Vector3f subLocal(float var)
	{
		this.x -= var;
		this.y -= var;
		this.z -= var;
		
		return this;
	}
	
	/****************************************************/
	// mul
	public Vector3f mul(Vector3f var)
	{
		return new Vector3f(x * var.x, y * var.y, z * var.z);
	}
	
	public Vector3f mul(Vector3f var, Vector3f result)
	{
		result.x = x * var.x;
		result.y = y * var.y;
		result.z = z * var.z;
		
		return result;
	}
	
	public Vector3f mul(float var)
	{
		return new Vector3f(x * var, y * var, z * var);
	}
	
	public Vector3f mul(float var, Vector3f result)
	{
		result.x = x * var;
		result.y = y * var;
		result.z = z * var;
		
		return result;
	}
	
	public Vector3f mulLocal(float var)
	{
		x *= var;
		y *= var;
		z *= var;
		
		return this;
	}
	
	public Vector3f mulLocal(Vector3f var)
	{
		x *= var.x;
		y *= var.y;
		z *= var.z;
		
		return this;
	}
	
	public Vector3f maddLocal(float multiplier, Vector3f adder)
	{
		x = x * multiplier + adder.x;
		y = y * multiplier + adder.y;
		z = x * multiplier + adder.z;
		
		return this;
	}
	
	public Vector3f negatived()
	{
		x *= -1;
		y *= -1;
		z *= -1;
		
		return this;
	}
	
	public Vector3f div(Vector3f r)
	{
		return new Vector3f(x / r.getX(), y / r.getY(), z / r.getZ());
	}
	
	public Vector3f div(float r)
	{
		return new Vector3f(x / r, y / r, z / r);
	}
	
	public Vector3f divLocal(float value)
	{
		x /= value;
		y /= value;
		z /= value;
		
		return this;
	}
	
	public Vector3f divLocal(Vector3f value)
	{
		x /= value.x;
		y /= value.y;
		z /= value.z;
		
		return this;
	}
	
	public Vector3f abs()
	{
		return new Vector3f(Math.abs(x), Math.abs(y), Math.abs(z));
	}
	
	public Vector3f absLocal()
	{
		x = Math.abs(x);
		y = Math.abs(y);
		z = Math.abs(z);
		
		return this;
	}
	
	public void calcOrthBasisAsYaxis(Vector3f out_xAxis, Vector3f out_zAxis)
	{
		if(Math.abs(y) < 0.9f)
		{
			out_xAxis.set(-z, 0.0f, x);// yAxis cross (0, 1, 0)
		}
		else
		{
			out_xAxis.set(y, -x, 0.0f);// yAxis cross (0, 0, 1)
		}
		
		out_xAxis.cross(this, out_zAxis);
		
		out_xAxis.normalizeLocal();
		out_zAxis.normalizeLocal();
	}
	
	public Vector3f clampLocal(float lowerBound, float upperBound)
	{
		x = Math.max(lowerBound, Math.min(x, upperBound));
		y = Math.max(lowerBound, Math.min(y, upperBound));
		z = Math.max(lowerBound, Math.min(z, upperBound));
		
		return this;
	}
	
	public Vector3f complement()
	{
		return new Vector3f(1.0f - x, 1.0f - y, 1.0f - z);
	}
	
	public float avg()
	{
		return (x + y + z) / 3.0f;
	}
	
	public Vector3f reflect(Vector3f normal)
	{
		Vector3f result = normal.mul(-2.0f * normal.dot(this));
		result.addLocal(this);
		return result;
	}
	
	public Vector3f reflectLocal(Vector3f normal)
	{
		float factor = -2.0f * normal.dot(this);
		
		x = x + factor * normal.x;
		y = y + factor * normal.y;
		z = z + factor * normal.z;
		
		return this;
	}
	
	// (x, y, z) = (min, mid, max)
	public void sort(Vector3f result)
	{
		if(x > y)
		{
			if(x > z)
			{
				result.z = x;
				
				if(y < z)
				{
					result.x = y;
					result.y = z;
				}
				else
				{
					result.x = z;
					result.y = y;
				}
			}
			else
			{
				result.z = z;
				result.y = x;
				result.x = y;
			}
		}
		else
		{
			if(x < z)
			{
				result.x = x;
				
				if(y > z)
				{
					result.z = y;
					result.y = z;
				}
				else
				{
					result.z = z;
					result.y = y;
				}
			}
			else
			{
				result.x = z;
				result.y = x;
				result.z = y;
			}
		}
	}
	
	@Override
	public String toString()
	{
//		DecimalFormat df = new DecimalFormat("000.00");
//		return "x: " + df.format(x) + " y: " + df.format(y) + " z: " + df.format(z);
		return "(" + x + ", " + y + ", " + z + ")";
	}
	
	public String toStringFormal()
	{
		return "(" + x + ", " + y + ", " + z + ")";
	}

	public Vector3f set(float x, float y, float z) { this.x = x; this.y = y; this.z = z; return this; }
	public Vector3f set(Vector3f r)                { set(r.x, r.y, r.z); return this; }

	public float getX() 
	{
		return x;
	}

	public void setX(float x) 
	{
		this.x = x;
	}

	public float getY() 
	{
		return y;
	}

	public void setY(float y) 
	{
		this.y = y;
	}

	public float getZ() 
	{
		return z;
	}

	public void setZ(float z) 
	{
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
        
        final Vector3f other = (Vector3f)obj;
		
		return x == other.x && 
			   y == other.y &&
			   z == other.z;
	}

	@Override
	public int hashCode()
	{
		int result = 7;

		result = 37 * result + Float.floatToIntBits(x);
		result = 37 * result + Float.floatToIntBits(y);
		result = 37 * result + Float.floatToIntBits(z);

		return result;
	}
}
