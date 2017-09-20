package appGui;

import appModel.EditorApp;
import appModel.GeneralOption;
import appModel.console.Console;
import appModel.console.MessageListener;
import appModel.event.ProjectEvent;
import appModel.event.ProjectEventListener;
import appModel.event.ProjectEventType;
import appModel.event.SettingListener;
import appModel.project.ProjectProxy;
import appModel.project.RenderSetting;
import appModel.project.TaskType;

import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import javax.imageio.ImageIO;

import appGui.util.FSBrowser;
import javafx.application.Platform;
import javafx.concurrent.Task;
import javafx.embed.swing.SwingFXUtils;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.control.Label;
import javafx.scene.control.ProgressBar;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;
import javafx.scene.control.TitledPane;
import javafx.scene.image.PixelWriter;
import javafx.scene.image.WritableImage;
import javafx.scene.input.KeyEvent;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.stage.FileChooser;
import photonApi.Frame;
import photonApi.FrameRegion;
import photonApi.FrameStatus;
import photonApi.Rectangle;
import photonApi.Statistics;
import photonApi.Vector3f;
import util.Time;

public class EditorCtrl
{
	// TODO: make project able to pickup directly typed text
	
	private ProjectProxy m_project;
    
    private WritableImage m_displayImage;
    
    private ProjectEventListener m_editorFinalFrameReadyListener;
	
	@FXML private VBox        projectOverviewVbox;
	@FXML private TitledPane  projectOverviewPane;
	@FXML private TextField   sceneFileTextField;
	@FXML private ProgressBar renderProgressBar;
	@FXML private Label       percentageProgressLabel;
	@FXML private AnchorPane  displayPane;
	@FXML private Canvas      canvas;
	@FXML private TextArea    messageTextArea;
	@FXML private Label       spsLabel;
	@FXML private Label       timeRemainingLabel;
    @FXML private Label       timeSpentLabel;
    
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
    	
    	m_editorFinalFrameReadyListener = (event) -> 
    	{
			clearFrame();
			loadFinalFrame();
			drawFrame();
    	};
    }
    
    public void startRenderingStaticScene()
    {
    	final double renderStartMs = Time.getTimeMs();
    	
		String sceneFileName = m_project.getRenderSetting().get(RenderSetting.SCENE_FILE_NAME);
		if(sceneFileName == null)
		{
			return;
		}
		
		final Task<String> loadSceneTask   = m_project.createTask(TaskType.LOAD_SCENE);
		final Task<String> renderTask      = m_project.createTask(TaskType.RENDER);
		final Task<String> developFilmTask = m_project.createTask(TaskType.DEVELOP_FILM);
		
		// TODO: exit when render task is done
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
				
				Statistics statistics = new Statistics();
				while(true)
				{
					m_project.asyncGetRendererStatistics(statistics);
					
					final long workDone  = (long)(statistics.percentageProgress + 0.5f);
					final long totalWork = 100;
					updateProgress(workDone, totalWork);
					
					final double workDoneFraction      = statistics.percentageProgress / 100.0;
					final double renderTimeMs          = Time.getTimeMs() - renderStartMs;
					final double totalRenderTimeMs     = renderTimeMs / workDoneFraction;
					final double remainingRenderTimeMs = totalRenderTimeMs * (1.0 - workDoneFraction);
					
					Platform.runLater(() -> 
					{
						percentageProgressLabel.setText(Float.toString(statistics.percentageProgress));
						spsLabel.setText(Long.toString((long)statistics.samplesPerSecond));
						timeSpentLabel.setText((long)(renderTimeMs / 1000.0) + " s");
						timeRemainingLabel.setText((long)(remainingRenderTimeMs / 1000.0) + " s");
					});
					
					if(workDone >= totalWork)
					{
						Platform.runLater(() -> percentageProgressLabel.setText("100"));
						break;
					}
					
					FrameRegion updatedFrameRegion = new FrameRegion();
					FrameStatus frameStatus = m_project.asyncGetUpdatedFrame(updatedFrameRegion);
					if(frameStatus != FrameStatus.INVALID)
					{
						Platform.runLater(() ->
						{
							loadFrameBuffer(updatedFrameRegion);
							drawFrame();
						});
					}
					
					try
					{
						Thread.sleep(1000);
					}
					catch(InterruptedException e)
					{
						e.printStackTrace();
					}
				}
				
				renderSceneThread.join();
				
				Thread developFilmThread = new Thread(developFilmTask);
				developFilmThread.start();
				developFilmThread.join();
				
				return "";
			}
		};
		
		renderProgressBar.progressProperty().bind(queryTask.progressProperty());
		
		Thread queryThread = new Thread(queryTask);
		queryThread.start();
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
			m_project.getRenderSetting().set(RenderSetting.SCENE_FILE_NAME, fileAbsPath);
		}
    }
    
    private void loadFinalFrame()
    {
    	final Frame frame = m_project.getLocalFinalFrame();
    	loadFrameBuffer(new FrameRegion(0, 0, frame.getWidthPx(), frame.getHeightPx(), frame));
    }
	
	private void loadFrameBuffer(FrameRegion frameRegion)
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
				
				// Tone-mapping operator: Jim Hejl and Richard Burgess-Dawson (GDC)
				// (no need of gamma correction)
				color.subLocal(0.004f).clampLocal(0.0f, Float.MAX_VALUE);
				Vector3f numerator   = color.mul(6.2f).addLocal(0.5f).mulLocal(color);
				Vector3f denominator = color.mul(6.2f).addLocal(1.7f).mulLocal(color).addLocal(0.06f);
				color.x = numerator.x / denominator.x;
				color.y = numerator.y / denominator.y;
				color.z = numerator.z / denominator.z;
				
				int inversedY = frameRegion.getHeightPx() - y - 1;
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
    
	private void drawFrame()
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
	
	public void setProject(ProjectProxy project)
	{
		if(m_project != null)
		{
			m_project.removeListener(m_editorFinalFrameReadyListener);
		}
		
		m_project = project;
		
		project.addListener(ProjectEventType.FINAL_FRAME_READY, m_editorFinalFrameReadyListener);
		project.getRenderSetting().addSettingListener((event) -> 
		{
			String sceneFilename = project.getRenderSetting().get(RenderSetting.SCENE_FILE_NAME);
			sceneFileTextField.setText(sceneFilename);
		});
	    	
		clearFrame();
		drawFrame();
		
		sceneFileTextField.setText(project.getRenderSetting().get(RenderSetting.SCENE_FILE_NAME));
	}
}
