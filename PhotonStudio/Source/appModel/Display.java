package appModel;

import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

import javax.imageio.ImageIO;

import javafx.embed.swing.SwingFXUtils;
import javafx.scene.image.Image;
import javafx.scene.image.PixelWriter;
import javafx.scene.image.WritableImage;
import javafx.scene.paint.Color;
import photonApi.FrameRegion;
import photonApi.Rectangle;
import util.Vector3f;

public class Display
{
	private WritableImage m_image;
	
	public Display()
	{
		m_image = new WritableImage(1, 1);
	}
	
	public void loadFrameBuffer(FrameRegion frameRegion)
	{
		if(!frameRegion.isValid() || frameRegion.getNumComp() != 3)
		{
			System.err.println("unexpected frame format; unable to load");
			return;
		}
		
		if(m_image.getWidth()  != frameRegion.getFullWidthPx() || 
		   m_image.getHeight() != frameRegion.getFullHeightPx())
		{
			m_image = new WritableImage(frameRegion.getFullWidthPx(), frameRegion.getFullHeightPx());
		}
		
		final PixelWriter pixelWriter = m_image.getPixelWriter();
		Rectangle region = frameRegion.getRegion();
		int maxX = region.x + region.w;
		int maxY = region.y + region.h;
		Vector3f color = new Vector3f();
		
		for(int y = region.y; y < maxY; y++)
		{
			for(int x = region.x; x < maxX; x++)
			{
				color.set(frameRegion.getRgb(x, y));
				if(!Float.isFinite(color.x) ||
				   !Float.isFinite(color.y) || 
				   !Float.isFinite(color.z))
				{
					System.err.println("color is not finite: " + color);
					color.set(0, 0, 0);
				}
				
				color.clampLocal(0.0f, 1.0f);
				
				int inversedY = frameRegion.getFullHeightPx() - y - 1;
				Color fxColor = new Color(color.x, color.y, color.z, 1.0);
				pixelWriter.setColor(x, inversedY, fxColor);
			}
		}
	}
	
	public void saveImage(String imageDirectory, String imageName)
    {
    	BufferedImage image = SwingFXUtils.fromFXImage(m_image, null);
    	try 
		{
		    File outputfile = new File(imageDirectory + imageName + ".png");
		    ImageIO.write(image, "png", outputfile);
		    
		    EditorApp.printToConsole("image saved");
		} 
		catch(IOException e)
		{
			e.printStackTrace();
			
			EditorApp.printToConsole("image saving failed");
		}
    }
	
	public int getWidth()
	{
		return (int)(m_image.getWidth());
	}
	
	public int getHeight()
	{
		return (int)(m_image.getHeight());
	}
	
	public Image getImage()
	{
		return m_image;
	}
}
