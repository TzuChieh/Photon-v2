package appGui;

import appModel.EditorApp;
import appModel.project.Project;

import java.io.IOException;
import java.util.HashMap;

import appGui.util.ChildWindow;
import appGui.util.MessagePopup;
import appGui.util.UILoader;
import appGui.util.ViewCtrlPair;
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
	private static final String ABOUT_FXML_FILENAME           = "About.fxml";
	
	private EditorApp             m_editorApp;
    private int                   m_projectId;
	private AppMainGraphicalState m_graphicalState;
	
	private Parent m_managerView;
	
	private HashMap<String, ViewCtrlPair<EditorCtrl>> m_editorUIs;
	private ManagerCtrl m_managerCtrl;
	private GeneralOptionsCtrl m_generalOptionsCtrl;
	
	private ChildWindow m_generalOptionsWindow;
	private ChildWindow m_aboutWindow;
	
	private UILoader m_uiLoader;
	
	@FXML private AnchorPane workbenchPane;
	@FXML private Pane       footerPane;
	@FXML private Button     renderBtn;
	@FXML private Label      footerMsgLbl;
    
    @FXML
    public void initialize()
    {
    	m_uiLoader = new UILoader();
    	
    	m_editorUIs = new HashMap<>();
    	
    	m_generalOptionsWindow = new ChildWindow();
    	m_aboutWindow          = new ChildWindow();
    	
    	footerPane.setBackground(new Background(new BackgroundFill(Color.BLACK, CornerRadii.EMPTY, Insets.EMPTY)));
//    	renderBtn.setBackground(new Background(new BackgroundFill(Color.RED, CornerRadii.EMPTY, Insets.EMPTY)));
    	
    	m_graphicalState = new AppMainGraphicalState(this);
    	
    	loadManagerUI();
    	loadGeneralOptionsUI();
    	loadAboutUI();
    }

	@FXML
	void newProjectBtnClicked(MouseEvent event)
	{
		// TODO: customizable project name
		final String newProjectName = "project " + m_projectId++;
		createNewProject(newProjectName);
		m_graphicalState.setActiveProject(newProjectName);
		setWorkbenchAsEditorView();
	}
	
	@FXML
	void saveImageBtnClicked(MouseEvent event)
	{
		ViewCtrlPair<EditorCtrl> editorUI = getActiveEditorUI();
		if(editorUI == null)
		{
			// TODO: log
			return;
		}
		
		// TODO: customizable image name
		String imageName = "result - " + m_graphicalState.getActiveProjectName();
		editorUI.getCtrl().saveDisplayImage(imageName);
	}
	
	@FXML
	void renderBtnClicked(MouseEvent event)
	{
		ViewCtrlPair<EditorCtrl> editorUI = getActiveEditorUI();
		if(editorUI == null)
		{
			// TODO: log
			return;
		}
		
		editorUI.getCtrl().startRenderingStaticScene();
	}
	
	@FXML
	void managerBtnClicked(MouseEvent event)
	{
		setWorkbenchAsManagerView();
	}
    
    @FXML
    void editorBtnClicked(MouseEvent event)
    {
    	setWorkbenchAsEditorView();
    }
    
    @FXML
    void generalOptionsClicked(ActionEvent event)
    {
    	m_generalOptionsWindow.show();
    }
    
    @FXML
    void aboutClicked(ActionEvent event)
    {
    	m_aboutWindow.show();
    }
    
    public AppMainCtrl()
    {
    	m_editorApp = null;
    	m_projectId = 0;
    }
    
    public void createNewProject(String projectName)
    {
    	Project project = m_editorApp.createProject(projectName);
    	if(project == null)
    	{
    		// TODO: log
    		return;
    	}
    	
    	ViewCtrlPair<EditorCtrl> editorUI = loadEditorUI();
    	if(!editorUI.isValid())
    	{
    		// TODO: log
    		return;
    	}
    	
    	m_editorUIs.put(projectName, editorUI);
    	
    	m_managerCtrl.registerProject(projectName);
    	editorUI.getCtrl().setProject(project);
    }
    
    public void setEditorApp(EditorApp editorApp)
    {
    	m_editorApp = editorApp;
    	
    	m_generalOptionsCtrl.setGeneralOption(editorApp.getGeneralOption());
    }
    
    public void setWorkbenchAsEditorView()
    {
    	setWorkbenchView(getActiveEditorUI().getView(), "project editor");
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
    
    private void loadManagerUI()
    {
    	try
		{
			FXMLLoader fxmlLoader = new FXMLLoader(getClass().getResource(MANAGER_FXML_FILENAME));
			
			m_managerView = fxmlLoader.load();
			m_managerCtrl = fxmlLoader.getController();
			
			m_managerCtrl.setAppMainGraphicalState(m_graphicalState);
		}
		catch(IOException e)
		{
			e.printStackTrace();
			new MessagePopup(e);
		}
    }
    
    private static ViewCtrlPair<EditorCtrl> loadEditorUI()
    {
    	return new UILoader().load(EditorCtrl.class.getResource(EDITOR_FXML_FILENAME));
    }
    
    private void loadGeneralOptionsUI()
    {
    	ViewCtrlPair<GeneralOptionsCtrl> ui = m_uiLoader.load(getClass().getResource(GENERAL_OPTIONS_FXML_FILENAME));
    	if(ui.isValid())
    	{
    		m_generalOptionsCtrl = ui.getCtrl();
			m_generalOptionsWindow.setContent(new Scene(ui.getView()));
			m_generalOptionsWindow.setTitle("General Options");
    	}
    }
    
    private void loadAboutUI()
    {
    	Parent view = m_uiLoader.loadView(getClass().getResource(ABOUT_FXML_FILENAME));
    	if(view != null)
    	{
    		m_aboutWindow.setContent(new Scene(view));
    		m_aboutWindow.setTitle("About");
    	}
    }
    
    private void setWorkbenchView(Parent view, String viewName)
    {
    	workbenchPane.getChildren().clear();
    	workbenchPane.getChildren().add(view);
    	
    	AnchorPane.setTopAnchor(view, 0.0);
    	AnchorPane.setBottomAnchor(view, 0.0);
    	AnchorPane.setLeftAnchor(view, 0.0);
    	AnchorPane.setRightAnchor(view, 0.0);
    	
    	m_graphicalState.setActiveViewName(viewName);
    }
    
    private ViewCtrlPair<EditorCtrl> getActiveEditorUI()
    {
    	String                   activeProject = m_graphicalState.getActiveProjectName();
    	ViewCtrlPair<EditorCtrl> editorUI      = m_editorUIs.get(activeProject);
    	
    	return editorUI;
    }
}
