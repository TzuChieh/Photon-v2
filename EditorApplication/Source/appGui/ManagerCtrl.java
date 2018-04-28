package appGui;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.scene.control.ListView;
import javafx.scene.control.ToggleButton;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.GridPane;

public class ManagerCtrl
{
	private AppMainGraphicalState  m_appMainGraphicalState;
	private ObservableList<String> m_projectNames;
	
	@FXML
	private BorderPane mainPane;
	
	@FXML
    private ListView<String> projectListView;
    
    @FXML
    public void initialize()
    {
		m_projectNames = FXCollections.observableArrayList();
		projectListView.setItems(m_projectNames);
		
		projectListView.getSelectionModel().selectedItemProperty().addListener(
		(observable, lastProjectName, currentProjectName) ->
		{
			m_appMainGraphicalState.setActiveProject(currentProjectName);
		});
    }
	    
    public void registerProject(String projectName)
    {
		m_projectNames.add(projectName);
		projectListView.getSelectionModel().select(projectName);
    }
	    
    public void setAppMainGraphicalState(AppMainGraphicalState state)
	{
		m_appMainGraphicalState = state;
	}
}