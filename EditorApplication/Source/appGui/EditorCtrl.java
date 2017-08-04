package appGui;

import appModel.EditorApp;
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
import photonApi.Vector3f;

public class EditorCtrl
{
	// TODO: make project able to pickup directly typed text
	
	private ProjectProxy m_project;
    
    private WritableImage m_displayImage;
    
    private ProjectEventListener m_editorFrameReadyListener;
	
	@FXML private VBox        projectOverviewVbox;
	@FXML private TitledPane  projectOverviewPane;
	@FXML private TextField   sceneFileTextField;
	@FXML private ProgressBar renderProgressBar;
	@FXML private Label       renderProgressLabel;
	@FXML private AnchorPane  displayPane;
	@FXML private Canvas      canvas;
	@FXML private TextArea    messageTextArea;
    
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
    	
    	m_editorFrameReadyListener = (event) -> 
    	{
			clearFrame();
			loadFrameBuffer();
			drawFrame();
    	};
    }
    
    public void startRenderingStaticScene()
    {
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
				
				while(true)
				{
					final float parametricProgress = m_project.queryParametricProgress();
					final float percentageProgress = parametricProgress * 100.0f;
					final long  workDone           = (long)(percentageProgress + 0.5f);
					final long  totalWork          = 100;
					updateProgress(workDone, totalWork);
					Platform.runLater(() -> renderProgressLabel.setText(percentageProgress + " %"));
					
					if(workDone >= totalWork)
					{
						Platform.runLater(() -> renderProgressLabel.setText("100 %"));
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
				
				renderSceneThread.join();
				
				Thread updateStaticImageThread = new Thread(developFilmTask);
				updateStaticImageThread.start();
				updateStaticImageThread.join();
				
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
		FileChooser chooser = new FileChooser();
		chooser.setTitle("Open Scene File");
			    
		File file = chooser.showOpenDialog(projectOverviewPane.getScene().getWindow());
		if(file != null)
		{
			try
			{
				String filename = file.getCanonicalPath();
				m_project.getRenderSetting().set(RenderSetting.SCENE_FILE_NAME, filename);
			}
			catch(IOException e)
			{
				e.printStackTrace();
			}
		}
    }
	
	private void loadFrameBuffer()
	{
		final Frame frame = m_project.getLocalFinalFrame();
		if(!frame.isValid() || frame.getNumComp() != 3)
		{
			System.err.println("unexpected frame format; unable to load");
			m_displayImage = new WritableImage(1, 1);
			return;
		}
		
		m_displayImage = new WritableImage(frame.getWidthPx(), frame.getHeightPx());
		final PixelWriter pixelWriter = m_displayImage.getPixelWriter();
		
		Vector3f color = new Vector3f();
		for(int y = 0; y < frame.getHeightPx(); y++)
		{
			for(int x = 0; x < frame.getWidthPx(); x++)
			{
				color.set(frame.getRgb(x, y));
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
				
				int inversedY = frame.getHeightPx() - y - 1;
				Color fxColor = new Color(color.x, color.y, color.z, 1.0);
				pixelWriter.setColor(x, inversedY, fxColor);
			}
		}
	}
	
//	private void loadFrameRegion(FrameRegion frameRegion)
//	{
//		m_displayImage = new WritableImage(frameRegion.getWpx(), frameRegion.getHpx());
//		final PixelWriter pixelWriter = m_displayImage.getPixelWriter();
//		
//		Vector3f color = new Vector3f();
//		for(int y = 0; y < frameRegion.getHpx(); y++)
//		{
//			for(int x = 0; x < frameRegion.getWpx(); x++)
//			{
//				color.set(frameRegion.getRgb(x, y));
//				if(color.x != color.x || 
//				   color.y != color.y || 
//				   color.z != color.z)
//				{
//					System.err.println("NaN!");
//				}
//				
//				// Tone-mapping operator: Jim Hejl and Richard Burgess-Dawson (GDC)
//				// (no need of gamma correction)
//				color.subLocal(0.004f).clampLocal(0.0f, Float.MAX_VALUE);
//				Vector3f numerator   = color.mul(6.2f).addLocal(0.5f).mulLocal(color);
//				Vector3f denominator = color.mul(6.2f).addLocal(1.7f).mulLocal(color).addLocal(0.06f);
//				color.x = numerator.x / denominator.x;
//				color.y = numerator.y / denominator.y;
//				color.z = numerator.z / denominator.z;
//				
//				int inversedY = frame.getHeightPx() - y - 1;
//				Color fxColor = new Color(color.x, color.y, color.z, 1.0);
//				pixelWriter.setColor(x, inversedY, fxColor);
//			}
//		}
//	}
	
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
			m_project.removeListener(m_editorFrameReadyListener);
		}
		
		m_project = project;
		
		project.addListener(ProjectEventType.STATIC_FRAME_READY, m_editorFrameReadyListener);
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
