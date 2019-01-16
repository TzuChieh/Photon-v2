package appModel;

import java.awt.image.BufferedImage;
import java.io.IOException;
import java.nio.file.Path;

import javax.imageio.ImageIO;

import photonApi.FrameRegion;

public abstract class Display
{
	private DisplayView m_view;
	
	protected Display()
	{
		m_view = new DisplayView(){};
	}
	
	public abstract void loadFrame(FrameRegion frameRegion);
	public abstract void drawFrame();
	public abstract void clearFrame();
	public abstract BufferedImage getImage();
	
	public void saveImage(Path filePath)
    {
    	BufferedImage image = getImage();
    	
    	String filename = filePath.getFileName().toString();
		int    dotIndex = filename.lastIndexOf('.');
		if(dotIndex != -1)
		{
			try 
			{
				String extension = filename.substring(dotIndex + 1);
				
			    ImageIO.write(image, extension.toLowerCase(), filePath.toFile());
			    
			    Studio.printToConsole("image saved to <" + filePath + ">");
			} 
			catch(IOException e)
			{
				e.printStackTrace();
				
				Studio.printToConsole("image saving failed");
			}
		}
		else
		{
			Studio.printToConsole("image saving failed, bad file path");
		}
    }
	
	protected DisplayView getDisplayView()
	{
		return m_view;
	}
	
	public void setDisplayView(DisplayView view)
	{
		m_view = view;
	}
}
