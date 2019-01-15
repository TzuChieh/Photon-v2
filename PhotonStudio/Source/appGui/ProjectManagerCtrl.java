package appGui;

import appModel.Studio;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.scene.control.ListView;
import javafx.scene.input.MouseButton;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.BorderPane;

public class ProjectManagerCtrl
{
	private ObservableList<String> m_projectNames;
	
	@FXML private BorderPane       mainPane;
	@FXML private ListView<String> projectListView;
	
	private Studio m_studio;
    
    @FXML
    public void initialize()
    {
		m_projectNames = FXCollections.observableArrayList();
		projectListView.setItems(m_projectNames);
    }
    
	@FXML
	void projectListClicked(MouseEvent event)
	{
		if(event.getButton().equals(MouseButton.PRIMARY))
		{
			if(event.getClickCount() == 2)
			{
				String projectName = projectListView.getSelectionModel().getSelectedItem();
				m_studio.setCurrentProject(projectName);
			}
		}
	}
	    
    public void addProject(String projectName)
    {
		m_projectNames.add(projectName);
		projectListView.getSelectionModel().select(projectName);
    }
    
    public void setStudio(Studio studio)
    {
    	m_studio = studio;
    }
}