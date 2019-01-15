package appGui;

import appModel.Project;
import appModel.Studio;
import appModel.StudioStatusView;
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
	private static final String PROJECT_MANAGER_FXML_PATH = "/fxmls/ProjectManager.fxml";
	private static final String RENDER_PROJECT_FXML_PATH  = "/fxmls/renderProject/RenderProject.fxml";
	private static final String MINECRAFT_FXML_PATH       = "/fxmls/Minecraft.fxml";
	private static final String GENERAL_OPTIONS_FXML_PATH = "/fxmls/GeneralOptions.fxml";
	private static final String ABOUT_FXML_PATH           = "/fxmls/About.fxml";
	
	private Studio             m_studio;
    private int                   m_projectId;
//	private AppMainGraphicalState m_graphicalState;
    private String m_workbenchName;
	
	private HashMap<String, ViewCtrlPair<RenderProjectCtrl>> m_projectUIs;
	private GeneralOptionsCtrl m_generalOptionsCtrl;
	private ViewCtrlPair<ProjectManagerCtrl> m_projectManagerUI;
	private ViewCtrlPair<MinecraftCtrl> m_minecraftUI;
	
	private ChildWindow m_generalOptionsWindow;
	private ChildWindow m_aboutWindow;
	
	private UILoader m_uiLoader;
	
	@FXML private AnchorPane workbenchPane;
	@FXML private Pane       footerPane;
	@FXML private Button     renderBtn;
	@FXML private Label      footerMessagLabel;
    
	public AppMainCtrl()
    {
    	m_studio = null;
    	m_projectId = 0;
    }
	
	@FXML
	public void initialize()
	{
		m_uiLoader = new UILoader();
		
		m_projectUIs = new HashMap<>();
		
		m_generalOptionsWindow = new ChildWindow();
		m_aboutWindow          = new ChildWindow();
		
		footerPane.setBackground(new Background(new BackgroundFill(Color.BLACK, CornerRadii.EMPTY, Insets.EMPTY)));
	//    	renderBtn.setBackground(new Background(new BackgroundFill(Color.RED, CornerRadii.EMPTY, Insets.EMPTY)));
		
//		m_graphicalState = new AppMainGraphicalState(this);
		m_workbenchName = "";
		
		loadGeneralOptionsUI();
		loadAboutUI();
		
		m_projectManagerUI = m_uiLoader.load(getClass().getResource(PROJECT_MANAGER_FXML_PATH));
//		m_managerUI.getCtrl().setAppMainGraphicalState(m_graphicalState);
		
		m_minecraftUI = m_uiLoader.load(getClass().getResource(MINECRAFT_FXML_PATH));
	}
	
	@FXML
	void editProjectBtnClicked(ActionEvent event)
	{
		setWorkbenchAsProjectView();
	}

	@FXML
	void editImageBtnClicked(ActionEvent event)
	{
	
	}
	
	@FXML
	void minecraftBtnClicked(ActionEvent event)
	{
		
	}
	
	@FXML
	void projectsBtnClicked(ActionEvent event)
	{
		setWorkbenchView(m_projectManagerUI.getView(), "project manager");
	}
	
	@FXML
	void renderBtnClicked(ActionEvent event)
	{
		// TODO: customizable project name
		final String newProjectName = "project " + m_projectId++;
		createNewProject(newProjectName);
//		m_graphicalState.setActiveProject(newProjectName);
		setWorkbenchAsProjectView();
	}
	
	@FXML
	void saveProjectBtnClicked(ActionEvent event)
	{
	
	}

//	@FXML
//	void newProjectBtnClicked(MouseEvent event)
//	{
//		// TODO: customizable project name
//		final String newProjectName = "project " + m_projectId++;
//		createNewProject(newProjectName);
////		m_graphicalState.setActiveProject(newProjectName);
//		setWorkbenchAsProjectView();
//	}
	
//	@FXML
//	void saveImageBtnClicked(MouseEvent event)
//	{
//		ViewCtrlPair<RenderProjectCtrl> projectUI = getCurrentProjectUI();
//		if(projectUI == null)
//		{
//			// TODO: log
//			return;
//		}
//		
//		// TODO: customizable image name
//		String imageName = "result - " + m_studio.getCurrentProject().getProjectSetting().getProjectName().getValue();
//		projectUI.getCtrl().saveDisplayImage(imageName);
//	}
	
//	@FXML
//	void renderBtnClicked(MouseEvent event)
//	{
//		ViewCtrlPair<RenderProjectCtrl> projectUI = getCurrentProjectUI();
//		if(projectUI == null)
//		{
//			// TODO: log
//			return;
//		}
//		
//		projectUI.getCtrl().startRenderingStaticScene();
//	}
	
//	@FXML
//	void managerBtnClicked(MouseEvent event)
//	{
//		
//	}
	
//	@FXML
//	void editorBtnClicked(MouseEvent event)
//	{
//		setWorkbenchAsProjectView();
//	}
	
//	@FXML
//	void minecraftBtnClicked(MouseEvent event)
//	{
//		
//	}
	
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
    
    public void createNewProject(String projectName)
    {
    	RenderProject project = m_studio.newRenderProject(projectName);
    	if(project == null)
    	{
    		System.err.println("error on creating project");
    		return;
    	}
    	
    	ViewCtrlPair<RenderProjectCtrl> projectUI = loadProjectUI();
    	if(!projectUI.isValid())
    	{
    		System.err.println("error on loading project UI");
    		return;
    	}
    	
    	projectUI.getCtrl().setProject(project);
    	m_projectUIs.put(projectName, projectUI);
    	m_projectManagerUI.getCtrl().addProject(projectName);
    	
    	m_studio.setCurrentProject(projectName);
    }
    
    public void setStudio(Studio studio)
    {
    	m_studio = studio;
    	
    	m_projectManagerUI.getCtrl().setStudio(studio);
    	m_generalOptionsCtrl.setGeneralOption(studio.getGeneralOption());
    	
    	m_studio.setStatusView(new StudioStatusView()
		{
    		@Override
    		public void showCurrentProject(Project project)
    		{
    			assert(project != null);
    			
    			setWorkbenchAsProjectView();
    			
    			footerMessagLabel.setText(
					"Project: "   + project.getProjectSetting().getProjectName().getValue() + " | " + 
                    "Workbench: " + m_workbenchName);
    		}
		});
    }
    
    public void setWorkbenchAsProjectView()
    {
    	setWorkbenchView(getCurrentProjectUI().getView(), "project editor");
    }
    
	private static ViewCtrlPair<RenderProjectCtrl> loadProjectUI()
	{
		return new UILoader().load(AppMainCtrl.class.getResource(RENDER_PROJECT_FXML_PATH));
	}
    
    private void loadGeneralOptionsUI()
    {
    	ViewCtrlPair<GeneralOptionsCtrl> ui = m_uiLoader.load(AppMainCtrl.class.getResource(GENERAL_OPTIONS_FXML_PATH));
    	if(ui.isValid())
    	{
    		m_generalOptionsCtrl = ui.getCtrl();
			m_generalOptionsWindow.setContent(new Scene(ui.getView()));
			m_generalOptionsWindow.setTitle("General Options");
    	}
    }
    
    private void loadAboutUI()
    {
    	Parent view = m_uiLoader.loadView(AppMainCtrl.class.getResource(ABOUT_FXML_PATH));
    	if(view != null)
    	{
    		m_aboutWindow.setContent(new Scene(view));
    		m_aboutWindow.setTitle("About");
    	}
    }
    
    private void setWorkbenchView(Parent view, String workbenchName)
    {
    	workbenchPane.getChildren().clear();
    	workbenchPane.getChildren().add(view);
    	
    	AnchorPane.setTopAnchor(view, 0.0);
    	AnchorPane.setBottomAnchor(view, 0.0);
    	AnchorPane.setLeftAnchor(view, 0.0);
    	AnchorPane.setRightAnchor(view, 0.0);
    	
    	m_workbenchName = workbenchName;
    }
    
    private ViewCtrlPair<RenderProjectCtrl> getCurrentProjectUI()
    {
    	assert(m_studio != null);
    	
    	String currentProject = m_studio.getCurrentProject().getProjectSetting().getProjectName().getValue();
    	return m_projectUIs.get(currentProject);
    }
}
