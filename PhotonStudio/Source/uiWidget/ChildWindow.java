package uiWidget;

import javafx.scene.Scene;
import javafx.stage.Modality;
import javafx.stage.Stage;

public class ChildWindow
{
	private Stage m_popupStage;
	
	public ChildWindow()
	{
		m_popupStage = new Stage();
    	m_popupStage.initModality(Modality.APPLICATION_MODAL);
    	
    	hide();
    	setTitle("Window");
	}
	
	public void setTitle(String title)
	{
		m_popupStage.setTitle(title);
	}
	
	public void setContent(Scene scene)
	{
		m_popupStage.setScene(scene);
	}
	
	public void show()
	{
		m_popupStage.show();
	}
	
	public void hide()
	{
		m_popupStage.hide();
	}
}
