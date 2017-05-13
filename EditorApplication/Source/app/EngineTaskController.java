package app;

import java.io.File;
import java.io.IOException;

import javafx.fxml.FXML;
import javafx.scene.control.TextField;
import javafx.scene.control.TitledPane;
import javafx.scene.input.MouseEvent;
import javafx.stage.FileChooser;

public class EngineTaskController
{
	@FXML
    private TitledPane engineTaskPane;
	
	@FXML
    private TextField sceneFileTextField;
	
	private PhotonDatabase m_database;
	
	@FXML
    void sceneFileBrowseBtnClicked(MouseEvent event)
	{
		//System.out.println(engineTaskPane);
		
		FileChooser chooser = new FileChooser();
	    chooser.setTitle("Open Scene File");
	    File file = chooser.showOpenDialog(engineTaskPane.getScene().getWindow());
	    if(file != null)
	    {
	    	try
			{
				sceneFileTextField.setText(file.getCanonicalPath());
			}
			catch(IOException e)
			{
				e.printStackTrace();
			}
	    }
	    
    }
	
	public void setDatabase(PhotonDatabase database)
    {
    	m_database = database;
    }
}
