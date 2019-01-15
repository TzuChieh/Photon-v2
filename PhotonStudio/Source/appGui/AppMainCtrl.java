package appGui;

import appModel.Studio;
import appModel.renderProject.RenderProject;

import java.util.HashMap;

import appGui.renderProject.RenderProjectCtrl;
import appGui.util.ChildWindow;
import appGui.util.UILoader;
import appGui.util.ViewCtrlPair;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
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

public class AppMainCtrl
{
	private static final String MANAGER_FXML_PATH         = "/fxmls/Manager.fxml";
	private static final String RENDER_PROJECT_FXML_PATH  = "/fxmls/renderProject/RenderProject.fxml";
	private static final String MINECRAFT_FXML_PATH       = "/fxmls/Minecraft.fxml";
	private static final String GENERAL_OPTIONS_FXML_PATH = "/fxmls/GeneralOptions.fxml";
	private static final String ABOUT_FXML_PATH           = "/fxmls/About.fxml";
	
	private Studio             m_studio;
    private int                   m_projectId;
	private AppMainGraphicalState m_graphicalState;
	
	private HashMap<String, ViewCtrlPair<RenderProjectCtrl>> m_editorUIs;
	private GeneralOptionsCtrl m_generalOptionsCtrl;
	private ViewCtrlPair<ManagerCtrl> m_managerUI;
	private ViewCtrlPair<MinecraftCtrl> m_minecraftUI;
	
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
		
		loadGeneralOptionsUI();
		loadAboutUI();
		
		m_managerUI = m_uiLoader.load(getClass().getResource(MANAGER_FXML_PATH));
		m_managerUI.getCtrl().setAppMainGraphicalState(m_graphicalState);
		
		m_minecraftUI = m_uiLoader.load(getClass().getResource(MINECRAFT_FXML_PATH));
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
		ViewCtrlPair<RenderProjectCtrl> editorUI = getActiveEditorUI();
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
		ViewCtrlPair<RenderProjectCtrl> editorUI = getActiveEditorUI();
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
		setWorkbenchView(m_managerUI.getView(), "project manager");
	}
	
	@FXML
	void editorBtnClicked(MouseEvent event)
	{
		setWorkbenchAsEditorView();
	}
	
	@FXML
	void minecraftBtnClicked(MouseEvent event)
	{
		setWorkbenchView(m_minecraftUI.getView(), "minecraft");
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
    	m_studio = null;
    	m_projectId = 0;
    }
    
    public void createNewProject(String projectName)
    {
    	RenderProject project = m_studio.newRenderProject(projectName);
    	if(project == null)
    	{
    		// TODO: log
    		return;
    	}
    	
    	ViewCtrlPair<RenderProjectCtrl> editorUI = loadEditorUI();
    	if(!editorUI.isValid())
    	{
    		// TODO: log
    		return;
    	}
    	
    	editorUI.getCtrl().setProject(project);
    	m_editorUIs.put(projectName, editorUI);
    	
    	m_managerUI.getCtrl().addProject(projectName);
    }
    
    public void setStudio(Studio studio)
    {
    	m_studio = studio;
    	m_generalOptionsCtrl.setGeneralOption(studio.getGeneralOption());
    }
    
    public void updateFooterText()
    {
    	footerMsgLbl.setText("Project: "   + m_graphicalState.getActiveProjectName() + " | " + 
    	                     "Workbench: " + m_graphicalState.getActiveViewName());
    }
    
    public void setWorkbenchAsEditorView()
    {
    	setWorkbenchView(getActiveEditorUI().getView(), "project editor");
    }
    
	private static ViewCtrlPair<RenderProjectCtrl> loadEditorUI()
	{
		return new UILoader().load(RenderProjectCtrl.class.getResource(RENDER_PROJECT_FXML_PATH));
	}
    
    private void loadGeneralOptionsUI()
    {
    	ViewCtrlPair<GeneralOptionsCtrl> ui = m_uiLoader.load(getClass().getResource(GENERAL_OPTIONS_FXML_PATH));
    	if(ui.isValid())
    	{
    		m_generalOptionsCtrl = ui.getCtrl();
			m_generalOptionsWindow.setContent(new Scene(ui.getView()));
			m_generalOptionsWindow.setTitle("General Options");
    	}
    }
    
    private void loadAboutUI()
    {
    	Parent view = m_uiLoader.loadView(getClass().getResource(ABOUT_FXML_PATH));
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
    
    private ViewCtrlPair<RenderProjectCtrl> getActiveEditorUI()
    {
    	String                   activeProject = m_graphicalState.getActiveProjectName();
    	ViewCtrlPair<RenderProjectCtrl> editorUI      = m_editorUIs.get(activeProject);
    	
    	return editorUI;
    }
}
