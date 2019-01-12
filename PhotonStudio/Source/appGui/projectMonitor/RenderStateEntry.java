package appGui.projectMonitor;

import javafx.application.Platform;
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
		Platform.runLater(() -> 
		{
			m_name.setText(name);
		});
	}
	
	public void setValue(String value)
	{
		Platform.runLater(() -> 
		{
			m_value.setText(value);
		});
	}
}
