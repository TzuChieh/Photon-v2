package appGui;

import java.io.File;
import java.io.IOException;

import javafx.fxml.FXML;
import javafx.scene.control.TextField;
import javafx.scene.control.TitledPane;
import javafx.scene.input.MouseEvent;
import javafx.stage.FileChooser;
import photonCore.PhEngine;

public class ProjectOverviewController
{
	@FXML
    private TitledPane engineTaskPane;
	
	@FXML
    private TextField sceneFileTextField;
	
	private EngineTaskModel m_model;
	
	@FXML
    void sceneFileBrowseBtnClicked(MouseEvent event)
	{
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

	@FXML
    void startRenderingBtnClicked(MouseEvent event)
    {
		String sceneFileName = sceneFileTextField.getText();
    	if(!sceneFileName.isEmpty())
    	{
    		m_model.loadSceneFile(sceneFileName);
    		m_model.renderScene();
    	}
    }
	
	public void setModel(EngineTaskModel model)
    {
		m_model = model;
		
		engineTaskPane.setText(model.getTaskName());
    }
}
