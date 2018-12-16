package util.minecraft;

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
		this(new Vector3f(0), new Vector3f(0));
	}
	
	public CuboidElement(Vector3f from, Vector3f to)
	{
		m_from = from;
		m_to   = to;
		
		m_rotOrigin  = new Vector3f(8.0f, 8.0f, 8.0f);
		m_rotAxis    = new Vector3f(0.0f, 1.0f, 0.0f);
		m_rotDegrees = 0.0f;
		
		m_faces = new Face[NUM_FACES];
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
		m_faces[facing.getValue()] = face;
	}
}
