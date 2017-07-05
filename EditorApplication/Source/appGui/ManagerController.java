package appGui;

import javafx.fxml.FXML;
import javafx.scene.layout.GridPane;

public class ManagerController
{
	private AppMainGraphicalState m_appMainGraphicalState;
	
    @FXML
    private GridPane projectBtnGrid;

    public ManagerController()
    {
    	
    }
    
    public void setAppMainGraphicalState(AppMainGraphicalState state)
    {
    	m_appMainGraphicalState = state;
    }
}