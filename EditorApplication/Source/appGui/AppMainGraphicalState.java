package appGui;

public final class AppMainGraphicalState
{
	private AppMainController m_appMainController;
	
	private String m_activeProjectName;
	private String m_activeViewName;
	
	public AppMainGraphicalState(AppMainController appMainConsroller)
	{
		m_appMainController = appMainConsroller;
		
		m_activeProjectName = "";
		m_activeViewName    = "";
	}
	
	public String getActiveProjectName()
	{
		return m_activeProjectName;
	}
	
	public String getActiveViewName()
	{
		return m_activeViewName;
	}
	
	public void setActiveViewName(String name)
	{
		m_activeViewName = name;
		m_appMainController.updateFooterText();
	}
	
	public void setActiveProjectName(String name)
	{
		m_activeProjectName = name;
		m_appMainController.updateFooterText();
	}
}
