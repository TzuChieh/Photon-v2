package app;

import java.io.IOException;

import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.VBox;

public class AppMainController
{
    @FXML
    private VBox engineTaskVbox;
    
    private PhotonDatabase m_database;

    @FXML
    void createNewTaskClicked(MouseEvent event)
    {
		try
		{
			FXMLLoader fxmlLoader = new FXMLLoader(getClass().getResource("EngineTask.fxml"));
			
			Parent               engineTaskUi         = fxmlLoader.load();
			EngineTaskController engineTaskController = fxmlLoader.getController();
			
			engineTaskController.setDatabase(m_database);
			
			engineTaskVbox.getChildren().add(engineTaskUi);
		}
		catch(IOException e)
		{
			e.printStackTrace();
		}
		
    }
    
    public void setDatabase(PhotonDatabase database)
    {
    	m_database = database;
    }
}
