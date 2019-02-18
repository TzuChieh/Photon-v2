package appModel;

import java.awt.image.BufferedImage;
import java.io.IOException;
import java.nio.file.Path;

import javax.imageio.ImageIO;

import photonApi.FrameRegion;
import photonApi.Rectangle;

public abstract class Display
{
	private DisplayInfoView m_infoView;
	
	protected Display()
	{
		m_infoView = new DisplayInfoView(){};
	}
	
	public abstract void loadFrame(FrameRegion frame);
	public abstract void drawFrame(Rectangle region);
	public abstract void drawIndicator(Rectangle region, int r255, int g255, int b255);
	public abstract void drawFlood();
	
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
	
	protected DisplayInfoView getDisplayInfoView()
	{
		return m_infoView;
	}
	
	public void setDisplayInfoView(DisplayInfoView view)
	{
		m_infoView = view;
	}
}
