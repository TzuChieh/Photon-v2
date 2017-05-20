package appGui;

import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

import javax.imageio.ImageIO;

import appModel.EditorApp;
import appModel.event.ProjectEvent;
import appModel.event.ProjectEventListener;
import appModel.event.ProjectEventType;
import appModel.project.Project;
import appModel.project.ProjectProxy;
import core.HdrFrame;
import core.Vector3f;
import javafx.embed.swing.SwingFXUtils;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.image.Image;
import javafx.scene.image.PixelWriter;
import javafx.scene.image.WritableImage;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import photonCore.FrameData;
import photonCore.PhEngine;

public class AppMainController
{
	private static final String FXML_VIEW_NAME = "ProjectOverview.fxml";
	
	@FXML
	private VBox projectOverviewVbox;
	
	@FXML
    private AnchorPane displayPane;
	
	@FXML
    private Canvas canvas;
	 
    private EditorApp m_editorApp;
    private int       m_projectId;
    
    private WritableImage m_displayImage;
    
    @FXML
    public void initialize()
    {
    	m_displayImage = new WritableImage(1, 1);
    	
    	canvas.widthProperty().addListener(observable -> drawFrame());
    	canvas.heightProperty().addListener(observable -> drawFrame());
    	canvas.widthProperty().bind(displayPane.widthProperty());
    	canvas.heightProperty().bind(displayPane.heightProperty());
    }

    @FXML
    void createNewProjectBtnClicked(MouseEvent event)
    {
    	createNewProject("project " + m_projectId++);
    }
    
    @FXML
    void saveImageBtnClicked(MouseEvent event)
    {
    	saveDisplayImage();
    }
    
    public AppMainController()
    {
    	m_editorApp = null;
    	m_projectId = 0;
    }
    
    public void createNewProject(String projectName)
    {
    	ProjectProxy project = m_editorApp.createProject(projectName);
    	project.addListener(ProjectEventType.STATIC_IMAGE_READY, new ProjectEventListener()
		{
			@Override
			public void onEventOccurred(ProjectEvent event)
			{
				final HdrFrame frame = new HdrFrame(event.source.getStaticImageData());
				
				m_displayImage = new WritableImage(frame.getWidthPx(), frame.getHeightPx());
				final PixelWriter pixelWriter = m_displayImage.getPixelWriter();
				
				Vector3f color   = new Vector3f();
				for(int y = 0; y < frame.getHeightPx(); y++)
				{
					for(int x = 0; x < frame.getWidthPx(); x++)
					{
						color.set(frame.getPixelR(x, y), 
						          frame.getPixelG(x, y), 
						          frame.getPixelB(x, y));
						if(color.x != color.x || 
						   color.y != color.y || 
						   color.z != color.z)
						{
							System.err.println("NaN!");
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
				
				drawFrame();
			}
		});
    	
    	try
		{
			FXMLLoader fxmlLoader = new FXMLLoader(getClass().getResource(FXML_VIEW_NAME));
			
			Parent                    projectOverviewView       = fxmlLoader.load();
			ProjectOverviewController projectOverviewController = fxmlLoader.getController();
			
			projectOverviewController.setProject(project);
			projectOverviewVbox.getChildren().add(projectOverviewView);
		}
		catch(IOException e)
		{
			e.printStackTrace();
			m_editorApp.deleteProject(projectName);
		}
    }
    
    public void setEditorApp(EditorApp editorApp)
    {
    	m_editorApp = editorApp;
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
		g.setFill(Color.DARKBLUE);
		g.fillRect(0, 0, canvas.getWidth(), canvas.getHeight());
		g.drawImage(m_displayImage, 
		            (canvas.getWidth() - imageWidth) * 0.5, (canvas.getHeight() - imageHeight) * 0.5, 
		            imageWidth, imageHeight);
    }
    
    private void saveDisplayImage()
    {
    	BufferedImage image = SwingFXUtils.fromFXImage(m_displayImage, null);
    	try 
		{
		    File outputfile = new File("./result.png");
		    ImageIO.write(image, "png", outputfile);
		    
		    System.out.println("image saved");
		} 
		catch(IOException e)
		{
			e.printStackTrace();
			
			System.err.println("image saving failed");
		}
    }
}
