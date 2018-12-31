package minecraft;

import util.Vector3f;

public class CuboidElement
{
	private static final int NUM_FACES = EFacing.values().length;
	
	private Vector3f m_from;
	private Vector3f m_to;
	
	private Vector3f m_rotOrigin;
	private Vector3f m_rotAxis;
	private float    m_rotDegrees;
	
	private Face[]   m_faces;
	
	public CuboidElement()
	{
		this(new Vector3f(0), new Vector3f(16));
	}
	
	public CuboidElement(Vector3f from, Vector3f to)
	{
		m_from = from;
		m_to   = to;
		
		m_rotOrigin  = from.add(to).div(2.0f);
		m_rotAxis    = new Vector3f(0.0f, 1.0f, 0.0f);
		m_rotDegrees = 0.0f;
		
		m_faces = new Face[NUM_FACES];
	}
	
	public Face getFace(EFacing facing)
	{
		return m_faces[facing.getValue()];
	}
	
	public Vector3f getMinVertex()
	{
		return m_from;
	}
	
	public Vector3f getMaxVertex()
	{
		return m_to;
	}
	
	public void setRotOrigin(Vector3f origin)
	{
		m_rotOrigin = origin;
	}
	
	public void setRotAxis(Vector3f axis)
	{
		m_rotAxis = axis;
	}
	
	public void setRotDegrees(float degrees)
	{
		m_rotDegrees = degrees;
	}
	
	public void setFace(EFacing facing, Face face)
	{
		// defaults to values equal to xyz position of the element
		if(!face.hasUV())
		{
			switch(facing)
			{
			case DOWN:
			case UP:
				face.setUVMin(m_from.z, m_from.x);
				face.setUVMax(m_to.z, m_to.x);
				break;
				
			case NORTH:
			case SOUTH:
				face.setUVMin(m_from.x, m_from.y);
				face.setUVMax(m_to.x, m_to.y);
				break;
				
			case EAST:
			case WEST:
				face.setUVMin(m_from.z, m_from.y);
				face.setUVMax(m_to.z, m_to.y);
				break;
			}
		}
		
		m_faces[facing.getValue()] = face;
	}
	
	@Override
	public String toString()
	{
		String result = "Cuboid Element: from " + m_from + ", to " + m_to;
		for(int f = 0; f < EFacing.SIZE; ++f)
		{
			Face face = m_faces[f];
			if(face != null)
			{
				result += "\n(" + EFacing.fromValue(f) + ") " + face;
			}
		}
		return result;
	}
}
