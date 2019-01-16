package appGui.renderProject;

import appModel.Display;
import appModel.Studio;
import appModel.GeneralOption;
import appModel.ProjectLogView;
import appModel.SettingListener;
import appModel.console.Console;
import appModel.console.MessageListener;
import appModel.renderProject.RenderFrameView;
import appModel.renderProject.RenderProject;

import java.nio.file.Paths;

import javax.imageio.ImageIO;

import appGui.widget.FSBrowser;
import appGui.widget.UILoader;
import appGui.widget.UI;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.control.Button;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.Label;
import javafx.scene.control.ProgressBar;
import javafx.scene.control.ProgressIndicator;
import javafx.scene.control.ScrollPane;
import javafx.scene.control.Spinner;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;
import javafx.scene.control.TitledPane;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import photonApi.Frame;
import photonApi.FrameRegion;

public class RenderProjectCtrl
{
	@FXML private VBox              projectOverviewVbox;
	@FXML private TitledPane        projectOverviewPane;
	@FXML private TextField         sceneFileTextField;
	@FXML private ProgressBar       renderProgressBar;
	@FXML private AnchorPane        displayPane;
	@FXML private Canvas            canvas;
	@FXML private TextArea          messageTextArea;
    @FXML private Spinner<Integer>  threadsSpinner;
    @FXML private ScrollPane        progressMonitorScrollPane;
	@FXML private ProgressIndicator renderingIndicator;
	@FXML private Label             renderingIndicatorLabel;
	@FXML private Button            startRenderingBtn;
	@FXML private TextField         imageFileSaveFolder;
	@FXML private ChoiceBox<String> imageFileSaveFormat;
	@FXML private TextField         imageFileSaveName;
	@FXML private AnchorPane        displayInfoPane;
    
    private RenderStatusCtrl m_renderProgressMonitor;
    private RenderFrameView  m_renderFrameView;
    private RenderProject    m_project;
    private Display          m_display;
    
    @FXML
    public void initialize()
    {
		m_display = new Display();
		
		renderingIndicator.setManaged(false);
		renderingIndicator.setVisible(false);
		
		ObservableList<String> imageFormats = FXCollections.observableArrayList();
		String[] supportedFormats = ImageIO.getWriterFormatNames();
		for(String supportedFormat : supportedFormats)
		{
			if(Character.isLowerCase(supportedFormat.charAt(0)))
			{
				imageFormats.add(supportedFormat);
			}
		}
		imageFileSaveFormat.setItems(imageFormats);
		imageFileSaveFormat.setValue("png");
		
		canvas.widthProperty().addListener(observable -> {clearFrame(); drawFrame();});
		canvas.heightProperty().addListener(observable -> {clearFrame(); drawFrame();});
		canvas.widthProperty().bind(displayPane.widthProperty());
		canvas.heightProperty().bind(displayPane.heightProperty());
		
		Studio.getConsole().addListener(new MessageListener()
		{
			@Override
			public void onMessageWritten(String message)
			{
				updateMessageTextArea();
			}
		});
		updateMessageTextArea();
		
		UI<RenderStatusCtrl> renderProgressMonitorUI = loadRenderProgressMonitorUI();
		progressMonitorScrollPane.setContent(renderProgressMonitorUI.getView());
		m_renderProgressMonitor = renderProgressMonitorUI.getCtrl();
		
		m_renderFrameView = new RenderFrameView()
		{
			@Override
			public void showIntermediate(FrameRegion frame)
			{
				loadFrameBuffer(frame);
				drawFrame();
			}
			
			@Override
			public void showFinal(Frame frame)
			{
				if(frame.isValid())
		    	{
		    		loadFrameBuffer(new FrameRegion(0, 0, frame.getWidthPx(), frame.getHeightPx(), frame));
		    		drawFrame();
		    	}
			}
		};
    }
    
    @FXML
    void sceneFileBrowseBtnClicked(MouseEvent event)
    {
    	String workingDirectory = m_project.getGeneralOption().get(GeneralOption.WORKING_DIRECTORY);
    	
    	FSBrowser browser = new FSBrowser(projectOverviewPane.getScene().getWindow());
    	browser.setBrowserTitle("Open Scene File");
    	browser.setStartingAbsDirectory(workingDirectory);
    	browser.startOpeningFile();
    	
    	String fileAbsPath = browser.getSelectedFileAbsPath();
		if(fileAbsPath != "")
		{
			m_project.getRenderSetting().getSceneFilePath().setValue(fileAbsPath);
			sceneFileTextField.setText(fileAbsPath);
		}
    }
    
	@FXML
	void startRenderingBtnClicked(ActionEvent event)
	{
		// TODO: load scene implicitly contains engine update, should make this explicit
		// TODO: better if we stop the rendering process on failed file loading
		
		final String oldText = renderingIndicatorLabel.getText();
		
		m_project.runLoadSceneTask(() -> 
		{
			renderingIndicatorLabel.setText("Loading Scene...");
			renderingIndicator.setManaged(true);
			renderingIndicator.setVisible(true);
			startRenderingBtn.setDisable(true);
		}, null);
		
		m_project.runRenderTask(() -> 
		{
			renderingIndicatorLabel.setText("Rendering Scene...");
		}, null);
		
		m_project.runUpdateStaticImageTask(null, () -> 
		{
			renderingIndicatorLabel.setText(oldText);
			renderingIndicator.setManaged(false);
			renderingIndicator.setVisible(false);
			startRenderingBtn.setDisable(false);
		});
	}
    
	@FXML
	void saveImageBtnClicked(ActionEvent event)
	{
		String folder    = imageFileSaveFolder.getText();
		String name      = imageFileSaveName.getText();
		String extension = imageFileSaveFormat.getValue();
		
		m_display.saveImage(Paths.get(folder + "/" + name + "." + extension));
	}
	
	@FXML
	void saveImageFolderBtnClicked(MouseEvent event)
	{
		String workingDirectory = m_project.getGeneralOption().get(GeneralOption.WORKING_DIRECTORY);
    	
    	FSBrowser browser = new FSBrowser(projectOverviewPane.getScene().getWindow());
    	browser.setBrowserTitle("Open Scene File");
    	browser.setStartingAbsDirectory(workingDirectory);
    	browser.startOpeningDirectory();
    	
    	String folderAbsPath = browser.getSelectedDirectoryAbsPath();
		if(folderAbsPath != "")
		{
			imageFileSaveFolder.setText(folderAbsPath);
		}
	}
	
	@FXML
	void transferImageToEditorBtnClicked(ActionEvent event)
	{
		// TODO
	}
	
    public void loadFrameBuffer(FrameRegion frameRegion)
	{
		m_display.loadFrameBuffer(frameRegion);
	}
	
	private void clearFrame()
	{
		GraphicsContext g = canvas.getGraphicsContext2D();
		g.setFill(Color.DARKBLUE);
		g.fillRect(0, 0, canvas.getWidth(), canvas.getHeight());
	}
    
	public void drawFrame()
	{
		final float canvasWidth       = (float)(canvas.getWidth());
		final float canvasHeight      = (float)(canvas.getHeight());
		final float canvasAspectRatio = canvasWidth / canvasHeight;
		final float frameAspectRatio  = (float)(m_display.getWidth()) / (float)(m_display.getHeight());
		
		int imageWidth;
		int imageHeight;
		if(frameAspectRatio > canvasAspectRatio)
		{
			imageWidth  = (int)canvasWidth;
			imageHeight = (int)(canvasWidth / frameAspectRatio);
		}
		else
		{
			imageHeight = (int)canvasHeight;
			imageWidth  = (int)(canvasHeight * frameAspectRatio);
		}
		
		GraphicsContext g = canvas.getGraphicsContext2D();
		g.drawImage(m_display.getImage(), 
		            (canvas.getWidth() - imageWidth) * 0.5, (canvas.getHeight() - imageHeight) * 0.5, 
		            imageWidth, imageHeight);
	}
    
    private void updateMessageTextArea()
    {
    	Console console = Studio.getConsole();
    	StringBuilder messages = new StringBuilder();
    	console.getCachedMessages(messages);
    	
//    	DefaultCaret caret = (DefaultCaret) messageTextArea.getCaret();
//    	caret.setUpdatePolicy(DefaultCaret.ALWAYS_UPDATE);
    	
    	messageTextArea.clear();
//    	messageTextArea.deleteText(0, messageTextArea.getText().length());
    	messageTextArea.setText(messages.toString());
    	messageTextArea.setScrollTop(Double.MAX_VALUE);
//    	messageTextArea.setScrollTop(Double.MAX_VALUE);
//    	messageTextArea.clear();
//    	messageTextArea.de
//    	messageTextArea.setScrollTop(Double.MAX_VALUE);
//    	messageTextArea.setText(messages.toString());
//    	messageTextArea.setScrollTop(Double.MAX_VALUE);
	}
	
	public void setProject(RenderProject project)
	{
		// TODO: if editor ctrl is reused, need to remove registered view or listeners
		
		m_project = project;
		
		String projectName = m_project.getProjectSetting().getProjectName().getValue();
		
		String imageName = "image - " + projectName;
		imageFileSaveName.setText(imageName);
		imageFileSaveFolder.setText("./");
		
		m_project.getRenderSetting().getSceneFilePath().addListener(new SettingListener()
    	{
			@Override
			public void onChanged(String name, String oldValue, String newValue)
			{
		    	sceneFileTextField.setText(newValue);
			}
    	});
		
		m_project.getRenderSetting().getNumThreads().addListener(new SettingListener()
    	{
			@Override
			public void onChanged(String name, String oldValue, String newValue)
			{
				int numThreads = Integer.parseInt(newValue);
		    	threadsSpinner.getValueFactory().setValue(numThreads);
			}
    	});
		
    	threadsSpinner.valueProperty().addListener((observable, oldValue, newValue) -> 
    		m_project.getRenderSetting().getNumThreads().setValue(Integer.toString(newValue)));
    	
    	clearFrame();
		if(project.getLocalFinalFrame().isValid())
		{
			Frame frame = project.getLocalFinalFrame();
			loadFrameBuffer(new FrameRegion(0, 0, frame.getWidthPx(), frame.getHeightPx(), frame));
			drawFrame();
		}
		
		m_project.setRenderFrameView(m_renderFrameView);
		m_project.setRenderStatusView(m_renderProgressMonitor.getRenderStatusView());
		
		m_project.setLogView(new ProjectLogView()
		{
			@Override
			public void showLog(String message)
			{
				Studio.printToConsole(message);
			}
		});
	}
	
	private static UI<RenderStatusCtrl> loadRenderProgressMonitorUI()
    {
    	return new UILoader().load(RenderProjectCtrl.class.getResource("/fxmls/renderProject/RenderStatus.fxml"));
    }
}
