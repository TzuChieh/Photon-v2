package appGui;

import appModel.EditorApp;
import appModel.project.ProjectProxy;

import java.io.IOException;

import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.geometry.Insets;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.Background;
import javafx.scene.layout.BackgroundFill;
import javafx.scene.layout.CornerRadii;
import javafx.scene.layout.Pane;
import javafx.scene.paint.Color;
import javafx.stage.Modality;
import javafx.stage.Stage;

public class AppMainCtrl
{
	private static final String MANAGER_FXML_FILENAME         = "Manager.fxml";
	private static final String EDITOR_FXML_FILENAME          = "Editor.fxml";
	private static final String GENERAL_OPTIONS_FXML_FILENAME = "GeneralOptions.fxml";
	
	private EditorApp m_editorApp;
    private int       m_projectId;
	private AppMainGraphicalState m_graphicalState;
	private Parent                m_managerView;
	private Parent                m_editorView;
	private ManagerCtrl        m_managerCtrl;
	private EditorCtrl         m_editorCtrl;
	private Scene m_generalOptionsScene;
	private Stage m_popupStage;
	
	@FXML private AnchorPane workbenchPane;
	@FXML private Pane       footerPane;
	@FXML private Button     renderBtn;
	@FXML private Label      footerMsgLbl;
    
    @FXML
    public void initialize()
    {
    	m_popupStage = new Stage();
    	m_popupStage.initModality(Modality.APPLICATION_MODAL);
    	m_popupStage.setOnHidden(event -> m_popupStage.setScene(null));
    	m_popupStage.hide();
    	
    	
    	footerPane.setBackground(new Background(new BackgroundFill(Color.BLACK, CornerRadii.EMPTY, Insets.EMPTY)));
//    	renderBtn.setBackground(new Background(new BackgroundFill(Color.RED, CornerRadii.EMPTY, Insets.EMPTY)));
    	
    	m_graphicalState = new AppMainGraphicalState(this);
    	
    	loadManagerView();
    	loadEditorView();
    	loadGeneralOptionsView();
    	
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
		m_editorCtrl.saveDisplayImage(imageName);
	}
	
	@FXML
	void renderBtnClicked(MouseEvent event)
	{
		m_editorCtrl.startRenderingStaticScene();
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
    	m_editorCtrl.setProject(m_editorApp.getProject(activeProjectName));
    	
    	setWorkbenchAsEditorView();
    }
    
    @FXML
    void generalOptionsClicked(ActionEvent event)
    {
    	showGeneralOptionsPopup();
    }
    
    public AppMainCtrl()
    {
    	m_editorApp = null;
    	m_projectId = 0;
    }
    
    public void createNewProject(String projectName)
    {
    	ProjectProxy project = m_editorApp.createProject(projectName);
    	m_managerCtrl.registerProject(projectName);
    	m_editorCtrl.setProject(project);
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
			FXMLLoader fxmlLoader = new FXMLLoader(getClass().getResource(MANAGER_FXML_FILENAME));
			
			m_managerView       = fxmlLoader.load();
			m_managerCtrl = fxmlLoader.getController();
			
			m_managerCtrl.setAppMainGraphicalState(m_graphicalState);
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
			FXMLLoader fxmlLoader = new FXMLLoader(getClass().getResource(EDITOR_FXML_FILENAME));
			
			m_editorView = fxmlLoader.load();
			m_editorCtrl = fxmlLoader.getController();
		}
		catch(IOException e)
		{
			e.printStackTrace();
			new MessagePopup(e);
		}
    }
    
    private void loadGeneralOptionsView()
    {
    	try
		{
			FXMLLoader fxmlLoader = new FXMLLoader(getClass().getResource(GENERAL_OPTIONS_FXML_FILENAME));
			
			Parent generalOptionsView = fxmlLoader.load();
			m_generalOptionsScene = new Scene(generalOptionsView);
		}
		catch(IOException e)
		{
			e.printStackTrace();
			new MessagePopup(e);
		}
    }
    
    private void showGeneralOptionsPopup()
    {
    	m_popupStage.setTitle("General Options");
    	m_popupStage.setScene(m_generalOptionsScene);
    	m_popupStage.show();
    }
}
