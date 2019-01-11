package appGui.util;

import javafx.scene.control.Label;

public class RenderStateEntry
{
	private static final int INTEGER = 1;
	private static final int REAL    = 2;
	
	private Label m_name;
	private Label m_value;
	private int   m_index;
	private int   m_type;
	
	public static RenderStateEntry newInteger(String name, int index)
	{
		return new RenderStateEntry(new Label(name), new Label(), index, INTEGER);
	}
	
	public static RenderStateEntry newReal(String name, int index)
	{
		return new RenderStateEntry(new Label(name), new Label(), index, REAL);
	}
	
	private RenderStateEntry(Label name, Label value, int index, int type)
	{
		m_name  = name;
		m_value = value;
		m_index = index;
		m_type  = type;
	}
	
	public Label getNameLabel()
	{
		return m_name;
	}
	
	public Label getValueLabel()
	{
		return m_value;
	}
	
	public int getIndex()
	{
		return m_index;
	}
	
	public boolean isEmpty()
	{
		return (m_name.getText() == null || m_name.getText().isEmpty()) &&
		       (m_value.getText() == null || m_value.getText().isEmpty());
	}
	
	public boolean isInteger()
	{
		return m_type == INTEGER;
	}
	
	public boolean isReal()
	{
		return m_type == REAL;
	}
	
	public void setName(String name)
	{
		m_name.setText(name);
	}
	
	public void setValue(String value)
	{
		m_value.setText(value);
	}
}
