package appGui.widget;

import javafx.scene.Parent;

public class UI<Ctrl>
{
	private Parent m_view;
	private Ctrl   m_ctrl;
	
	public UI()
	{
		this(null, null);
	}
	
	public UI(Parent view, Ctrl ctrl)
	{
		m_view = view;
		m_ctrl = ctrl;
	}
	
	public Parent getView()
	{
		return m_view;
	}
	
	public Ctrl getCtrl()
	{
		return m_ctrl;
	}
	
	public boolean isValid()
	{
		return m_view != null && m_ctrl != null;
	}
}
