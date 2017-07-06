package appGui;

import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.imageio.ImageIO;
import javax.swing.text.DefaultCaret;

import appModel.EditorApp;
import appModel.console.Console;
import appModel.console.MessageListener;
import appModel.event.ProjectEvent;
import appModel.event.ProjectEventListener;
import appModel.event.ProjectEventType;
import appModel.project.Project;
import appModel.project.ProjectProxy;
import core.HdrFrame;
import core.Vector3f;
import javafx.application.Platform;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.embed.swing.SwingFXUtils;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.geometry.Insets;
import javafx.scene.Parent;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.control.Label;
import javafx.scene.control.TextArea;
import javafx.scene.image.Image;
import javafx.scene.image.PixelWriter;
import javafx.scene.image.WritableImage;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.Background;
import javafx.scene.layout.BackgroundFill;
import javafx.scene.layout.CornerRadii;
import javafx.scene.layout.Pane;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import photonCore.FrameData;
import photonCore.PhEngine;

public class AppMainController
{
	private static final String FXML_MANAGER_NAME = "Manager.fxml";
	private static final String FXML_EDITOR_NAME  = "Editor.fxml";
	
	private EditorApp m_editorApp;
    private int       m_projectId;
	private AppMainGraphicalState m_graphicalState;
	private Parent                m_managerView;
	private Parent                m_editorView;
	private ManagerController     m_managerController;
	private EditorController      m_editorController;
	
	@FXML
    private AnchorPane workbenchPane;
	
	@FXML
    private Pane footerPane;
	
	@FXML
    private Label footerMsgLbl;
    
    @FXML
    public void initialize()
    {
    	footerPane.setBackground(new Background(new BackgroundFill(Color.BLACK, CornerRadii.EMPTY, Insets.EMPTY)));
    	
    	m_graphicalState = new AppMainGraphicalState(this);
    	
    	loadManagerView();
    	loadEditorView();
    	
    	setWorkbenchAsEditorView();
    }

	@FXML
	void newProjectBtnClicked(MouseEvent event)
	{
		final String newProjectName = "project " + m_projectId++;
		createNewProject(newProjectName);
	}
	
	@FXML
	void saveImageBtnClicked(MouseEvent event)
	{
		String imageName = "result - " + m_graphicalState.getActiveProjectName();
		m_editorController.saveDisplayImage(imageName);
	}
	
	@FXML
	void renderBtnClicked(MouseEvent event)
	{
		m_editorController.startRenderingStaticScene();
	}
	
	@FXML
	void managerBtnClicked(MouseEvent event)
	{
		setWorkbenchAsManagerView();
	}
    
    @FXML
    void editorBtnClicked(MouseEvent event)
    {
    	String activeProjectName = m_graphicalState.getActiveProjectName();
    	m_editorController.setProject(m_editorApp.getProject(activeProjectName));
    	
    	setWorkbenchAsEditorView();
    }
    
    public AppMainController()
    {
    	m_editorApp = null;
    	m_projectId = 0;
    }
    
    public void createNewProject(String projectName)
    {
    	ProjectProxy project = m_editorApp.createProject(projectName);
    	m_managerController.registerProject(projectName);
    	m_editorController.setProject(project);
//    	try
//		{
//			FXMLLoader fxmlLoader = new FXMLLoader(getClass().getResource(FXML_MANAGER_NAME));
//			
//			Parent            managerView       = fxmlLoader.load();
//			ManagerController managerController = fxmlLoader.getController();
//			
//			managerController.setProject(project);
//			projectOverviewVbox.getChildren().add(managerView);
//		}
//		catch(IOException e)
//		{
//			e.printStackTrace();
//			m_editorApp.deleteProject(projectName);
//		}
    }
    
    public void setEditorApp(EditorApp editorApp)
    {
    	m_editorApp = editorApp;
    }
    
    public void setWorkbenchAsEditorView()
    {
    	workbenchPane.getChildren().clear();
    	workbenchPane.getChildren().add(m_editorView);
    	
    	AnchorPane.setTopAnchor(m_editorView, 0.0);
    	AnchorPane.setBottomAnchor(m_editorView, 0.0);
    	AnchorPane.setLeftAnchor(m_editorView, 0.0);
    	AnchorPane.setRightAnchor(m_editorView, 0.0);
    	
    	m_graphicalState.setActiveViewName("project editor");
    }
    
    public void setWorkbenchAsManagerView()
    {
    	workbenchPane.getChildren().clear();
    	workbenchPane.getChildren().add(m_managerView);
    	
    	AnchorPane.setTopAnchor(m_managerView, 0.0);
    	AnchorPane.setBottomAnchor(m_managerView, 0.0);
    	AnchorPane.setLeftAnchor(m_managerView, 0.0);
    	AnchorPane.setRightAnchor(m_managerView, 0.0);
    	
    	m_graphicalState.setActiveViewName("project manager");
    }
    
    public void updateFooterText()
    {
    	footerMsgLbl.setText("Project: "   + m_graphicalState.getActiveProjectName() + " | " + 
    	                     "Workbench: " + m_graphicalState.getActiveViewName());
    }
    
    private void loadManagerView()
    {
    	try
		{
			FXMLLoader fxmlLoader = new FXMLLoader(getClass().getResource(FXML_MANAGER_NAME));
			
			m_managerView       = fxmlLoader.load();
			m_managerController = fxmlLoader.getController();
			
			m_managerController.setAppMainGraphicalState(m_graphicalState);
		}
		catch(IOException e)
		{
			e.printStackTrace();
			new MessagePopup(e);
		}
    }
    
    private void loadEditorView()
    {
    	try
		{
			FXMLLoader fxmlLoader = new FXMLLoader(getClass().getResource(FXML_EDITOR_NAME));
			
			m_editorView       = fxmlLoader.load();
			m_editorController = fxmlLoader.getController();
		}
		catch(IOException e)
		{
			e.printStackTrace();
			new MessagePopup(e);
		}
    }
}
