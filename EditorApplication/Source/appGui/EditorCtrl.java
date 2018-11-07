package appGui;

import appModel.EditorApp;
import appModel.GeneralOption;
import appModel.console.Console;
import appModel.console.MessageListener;
import appModel.event.ProjectEventType;
import appModel.project.Project;
import appModel.project.RenderSetting;

import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicInteger;

import javax.imageio.ImageIO;

import appGui.util.FSBrowser;
import appGui.util.UILoader;
import appGui.util.ViewCtrlPair;
import javafx.application.Platform;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.collections.FXCollections;
import javafx.concurrent.Task;
import javafx.embed.swing.SwingFXUtils;
import javafx.fxml.FXML;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.Label;
import javafx.scene.control.ProgressBar;
import javafx.scene.control.ScrollPane;
import javafx.scene.control.Spinner;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;
import javafx.scene.control.TitledPane;
import javafx.scene.image.PixelWriter;
import javafx.scene.image.WritableImage;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import photonApi.Frame;
import photonApi.FrameRegion;
import photonApi.FrameStatus;
import photonApi.Ph;
import photonApi.Rectangle;
import photonApi.RenderState;
import photonApi.Statistics;
import photonApi.Vector3f;
import util.Time;

public class EditorCtrl
{
	// TODO: make project able to pickup directly typed text
	
	private Project m_project;
    
    private WritableImage m_displayImage;
	
	@FXML private VBox             projectOverviewVbox;
	@FXML private TitledPane       projectOverviewPane;
	@FXML private TextField        sceneFileTextField;
	@FXML private ProgressBar      renderProgressBar;
	@FXML private AnchorPane       displayPane;
	@FXML private Canvas           canvas;
	@FXML private TextArea         messageTextArea;
    @FXML private Spinner<Integer> threadsSpinner;
	@FXML private AnchorPane       progressMonitorPane;
    
    private RenderProgressMonitorCtrl m_renderProgressMonitor;
    
    @FXML
    public void initialize()
    {
    	m_displayImage = new WritableImage(1, 1);
    	
    	canvas.widthProperty().addListener(observable -> {clearFrame(); drawFrame();});
    	canvas.heightProperty().addListener(observable -> {clearFrame(); drawFrame();});
    	canvas.widthProperty().bind(displayPane.widthProperty());
    	canvas.heightProperty().bind(displayPane.heightProperty());
    	
    	EditorApp.getConsole().addListener(new MessageListener()
		{
			@Override
			public void onMessageWritten(String message)
			{
				Platform.runLater(() -> updateMessageTextArea());
			}
		});
    	updateMessageTextArea();
    	
    	ViewCtrlPair<RenderProgressMonitorCtrl> renderProgressMonitorUI = loadRenderProgressMonitorUI();
    	progressMonitorPane.getChildren().clear();
    	progressMonitorPane.getChildren().add(renderProgressMonitorUI.getView());
    	AnchorPane.setTopAnchor(renderProgressMonitorUI.getView(), 0.0);
    	AnchorPane.setBottomAnchor(renderProgressMonitorUI.getView(), 0.0);
    	AnchorPane.setLeftAnchor(renderProgressMonitorUI.getView(), 0.0);
    	AnchorPane.setRightAnchor(renderProgressMonitorUI.getView(), 0.0);
    	m_renderProgressMonitor = renderProgressMonitorUI.getCtrl();
    	m_renderProgressMonitor.setDisplay(this);
    }
    
    public void startRenderingStaticScene()
    {
		String sceneFileName = m_project.getRenderSetting().get(RenderSetting.SCENE_FILE_PATH);
		if(sceneFileName == null)
		{
			return;
		}
		
		ExecutorService executor = Executors.newSingleThreadExecutor();
		{
			// TODO: load scene implicitly contains engine update, should make this explicit
			Task<Void> loadSceneTask = m_project.createLoadSceneTask();
			loadSceneTask.setOnSucceeded((event) -> {
				m_renderProgressMonitor.startMonitoring();
			});
			executor.submit(loadSceneTask);
			
			Task<Void> renderTask = m_project.createRenderTask();
			renderTask.setOnSucceeded((event) -> {
				m_renderProgressMonitor.stopMonitoring();
			});
			renderTask.setOnFailed((event) -> {
				m_renderProgressMonitor.stopMonitoring();
			});
			executor.submit(renderTask);
			
			Task<Void> developFilmTask = m_project.createUpdateStaticImageTask();
			executor.submit(developFilmTask);
		}
		executor.shutdown();
    }
    
    @FXML
    void sceneFileBrowseBtnClicked(MouseEvent event)
    {
    	String workingDirectory = m_project.getGeneralOption().get(GeneralOption.WORKING_DIRECTORY);
    	
    	FSBrowser browser = new FSBrowser(projectOverviewPane.getScene().getWindow());
    	browser.setBrowserTitle("Open Scene File");
    	browser.setStartingAbsDirectory(workingDirectory);
    	browser.startBrowsingFile();
    	
    	String fileAbsPath = browser.getSelectedFileAbsPath();
		if(fileAbsPath != "")
		{
			m_project.getRenderSetting().set(RenderSetting.SCENE_FILE_PATH, fileAbsPath);
		}
    }
    
    public void loadFinalFrame()
    {
    	final Frame frame = m_project.getLocalFinalFrame();
    	if(frame.isValid())
    	{
    		loadFrameBuffer(new FrameRegion(0, 0, frame.getWidthPx(), frame.getHeightPx(), frame));
    	}
    }
	
    public void loadFrameBuffer(FrameRegion frameRegion)
	{
		if(!frameRegion.isValid() || frameRegion.getNumComp() != 3)
		{
			System.err.println("unexpected frame format; unable to load");
			return;
		}
		
		if(m_displayImage.getWidth()  != frameRegion.getFullWidthPx() || 
		   m_displayImage.getHeight() != frameRegion.getFullHeightPx())
		{
			m_displayImage = new WritableImage(frameRegion.getFullWidthPx(), frameRegion.getFullHeightPx());
		}
		
		final PixelWriter pixelWriter = m_displayImage.getPixelWriter();
		Rectangle region = frameRegion.getRegion();
		int maxX = region.x + region.w;
		int maxY = region.y + region.h;
		Vector3f color = new Vector3f();
		
		for(int y = region.y; y < maxY; y++)
		{
			for(int x = region.x; x < maxX; x++)
			{
				color.set(frameRegion.getRgb(x, y));
				if(color.x != color.x || 
				   color.y != color.y || 
				   color.z != color.z)
				{
					System.err.println("NaN in frame");
					color.set(0, 0, 0);
				}
				
				color.clampLocal(0.0f, 1.0f);
				
				int inversedY = frameRegion.getFullHeightPx() - y - 1;
				Color fxColor = new Color(color.x, color.y, color.z, 1.0);
				pixelWriter.setColor(x, inversedY, fxColor);
			}
		}
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
		final float frameAspectRatio  = (float)(m_displayImage.getWidth()) / (float)(m_displayImage.getHeight());
		
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
		g.drawImage(m_displayImage, 
		            (canvas.getWidth() - imageWidth) * 0.5, (canvas.getHeight() - imageHeight) * 0.5, 
		            imageWidth, imageHeight);
	}
    
    public void saveDisplayImage(String imageName)
    {
    	BufferedImage image = SwingFXUtils.fromFXImage(m_displayImage, null);
    	try 
		{
		    File outputfile = new File("./" + imageName + ".png");
		    ImageIO.write(image, "png", outputfile);
		    
		    EditorApp.printToConsole("image saved");
		} 
		catch(IOException e)
		{
			e.printStackTrace();
			
			EditorApp.printToConsole("image saving failed");
		}
    }
    
    private void updateMessageTextArea()
    {
    	Console console = EditorApp.getConsole();
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
	
	public void associateWithProject(Project project)
	{
		assert(m_project == null);
		
		m_project = project;
		
		project.addListener(ProjectEventType.FINAL_FRAME_READY, (event) -> 
    	{
			clearFrame();
			loadFinalFrame();
			drawFrame();
    	});
		
		project.getRenderSetting().addSettingListener((event) ->
    	{
    		if(event.settingName.equals(RenderSetting.SCENE_FILE_PATH))
    		{
    			sceneFileTextField.setText(event.newSettingValue);
    		}
    	});
		
		int numRenderThreads = Integer.parseInt(project.getRenderSetting().get(RenderSetting.NUM_RENDER_THREADS));
    	threadsSpinner.getValueFactory().setValue(numRenderThreads);
    	threadsSpinner.valueProperty().addListener((observable, oldValue, newValue) -> project.setNumRenderThreads(newValue));
    	
    	sceneFileTextField.setText(project.getRenderSetting().get(RenderSetting.SCENE_FILE_PATH));
    	
    	clearFrame();
		if(project.getLocalFinalFrame().isValid())
		{
			loadFinalFrame();
			drawFrame();
		}
		
		m_renderProgressMonitor.setMonitoredProject(m_project);
	}
	
	private static ViewCtrlPair<RenderProgressMonitorCtrl> loadRenderProgressMonitorUI()
    {
    	return new UILoader().load(EditorCtrl.class.getResource("/fxmls/RenderProgressMonitor.fxml"));
    }
}
