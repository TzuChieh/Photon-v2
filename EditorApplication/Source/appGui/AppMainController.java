package appGui;

import java.io.IOException;

import appModel.EditorApp;
import appModel.project.Project;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.VBox;
import photonCore.PhEngine;

public class AppMainController
{
	@FXML
	private VBox projectOverviewVbox;
	 
    private EditorApp m_editorApp;
    private int       m_projectId;

    @FXML
    void createNewProjectBtnClicked(MouseEvent event)
    {
    	createNewProject("project " + m_projectId++);
    }
    
    public AppMainController()
    {
    	m_editorApp = null;
    	m_projectId = 0;
    }
    
    public void createNewProject(String taskName)
    {
    	try
		{
    		Project project = new Project();
    		
			FXMLLoader fxmlLoader = new FXMLLoader(getClass().getResource("EngineTask.fxml"));
			
			Parent                    projectOverviewView       = fxmlLoader.load();
			ProjectOverviewController projectOverviewController = fxmlLoader.getController();
			
			engineTaskModel.setTaskName(taskName);
			engineTaskController.setModel(engineTaskModel);
			projectOverviewVbox.getChildren().add(engineTaskView);
			
			m_model.addEngineTaskModel(taskName, engineTaskModel);
		}
		catch(IOException e)
		{
			e.printStackTrace();
		}
    }
    
    public void setEditorApp(EditorApp editorApp)
    {
    	m_editorApp = editorApp;
    }
}
