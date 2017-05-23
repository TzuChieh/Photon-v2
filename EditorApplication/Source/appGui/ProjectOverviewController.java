package appGui;

import java.io.File;
import java.io.IOException;

import appModel.event.SettingEvent;
import appModel.event.SettingListener;
import appModel.project.ProjectProxy;
import appModel.project.RenderSetting;
import appModel.project.TaskType;
import javafx.concurrent.Task;
import javafx.fxml.FXML;
import javafx.scene.control.ProgressBar;
import javafx.scene.control.TextField;
import javafx.scene.control.TitledPane;
import javafx.scene.input.MouseEvent;
import javafx.stage.FileChooser;
import photonCore.PhEngine;

public class ProjectOverviewController
{
	@FXML
    private TitledPane projectOverviewPane;
	
	@FXML
    private TextField sceneFileTextField;
	
	@FXML
    private ProgressBar renderProgressBar;
	
	private ProjectProxy m_project;
	
	@FXML
    void sceneFileBrowseBtnClicked(MouseEvent event)
	{
		FileChooser chooser = new FileChooser();
	    chooser.setTitle("Open Scene File");
	    
	    File file = chooser.showOpenDialog(projectOverviewPane.getScene().getWindow());
	    if(file != null)
	    {
	    	try
			{
	    		String filename = file.getCanonicalPath();
	    		sceneFileTextField.setText(filename);
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
		String sceneFileName = m_project.getRenderSetting().get(RenderSetting.SCENE_FILE_NAME);
    	if(sceneFileName != null)
    	{
    		m_project.getRenderSetting().set(RenderSetting.SCENE_FILE_NAME, sceneFileName);
    		
    		final Task<String> loadSceneTask         = m_project.createTask(TaskType.LOAD_SCENE);
    		final Task<String> renderTask            = m_project.createTask(TaskType.RENDER);
    		final Task<String> updateStaticImageTask = m_project.createTask(TaskType.UPDATE_STATIC_IMAGE);
    		
    		final Task<String> queryTask = new Task<String>()
			{
				@Override
				protected String call() throws Exception
				{
					Thread loadSceneThread = new Thread(loadSceneTask);
					loadSceneThread.start();
					loadSceneThread.join();
					
					Thread renderSceneThread = new Thread(renderTask);
					renderSceneThread.start();
					
					while(true)
					{
						final float parametricProgress = m_project.queryParametricProgress();
						final long  workDone           = (long)(parametricProgress * 100.0f + 0.5f);
						final long  totalWork          = 100;
						updateProgress(workDone, totalWork);
						
						if(workDone >= totalWork)
						{
							break;
						}
						
						try
						{
							Thread.sleep(3000);
						}
						catch(InterruptedException e)
						{
							e.printStackTrace();
						}
					}
					
					Thread updateStaticImageThread = new Thread(updateStaticImageTask);
					updateStaticImageThread.start();
					updateStaticImageThread.join();
					
					return "";
				}
			};
    		
			renderProgressBar.progressProperty().bind(queryTask.progressProperty());
			
			Thread queryThread = new Thread(queryTask);
			queryThread.start();
    	}
    }
	
	public void setProject(ProjectProxy project)
    {
		m_project = project;
		
		sceneFileTextField.textProperty().addListener((observable, oldValue, newValue) -> 
		{
			m_project.getRenderSetting().set(RenderSetting.SCENE_FILE_NAME, newValue);
		});
    }
}
