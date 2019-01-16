package appModel;

import java.awt.image.BufferedImage;
import java.io.IOException;
import java.nio.file.Path;

import javax.imageio.ImageIO;

import javafx.embed.swing.SwingFXUtils;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.image.Image;
import javafx.scene.image.PixelWriter;
import javafx.scene.image.WritableImage;
import javafx.scene.paint.Color;
import photonApi.FrameRegion;
import photonApi.Rectangle;
import util.Vector3f;

public class StaticCanvasDisplay extends Display
{
	private WritableImage m_image;
	private Canvas        m_canvas;
	
	public StaticCanvasDisplay()
	{
		this(1, 1);
	}
	
	public StaticCanvasDisplay(int widthPx, int heightPx)
	{
		super();
		
		m_image = new WritableImage(widthPx, heightPx);
		m_canvas = new Canvas();
		
		getDisplayView().showResolution(widthPx, heightPx);
	}
	
	@Override
	public void loadFrame(FrameRegion frameRegion)
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
			
			getDisplayView().showResolution(frameRegion.getFullWidthPx(), frameRegion.getFullHeightPx());
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
	
	@Override
	public void drawFrame()
	{
		final float canvasWidth       = (float)(m_canvas.getWidth());
		final float canvasHeight      = (float)(m_canvas.getHeight());
		final float canvasAspectRatio = canvasWidth / canvasHeight;
		final float frameAspectRatio  = (float)(m_image.getWidth()) / (float)(m_image.getHeight());
		
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
		
		GraphicsContext g = m_canvas.getGraphicsContext2D();
		g.drawImage(m_image, 
		            (m_canvas.getWidth() - imageWidth) * 0.5, (m_canvas.getHeight() - imageHeight) * 0.5, 
		            imageWidth, imageHeight);
	}
	
	@Override
	public void clearFrame()
	{
		GraphicsContext g = m_canvas.getGraphicsContext2D();
		g.setFill(Color.DARKBLUE);
		g.fillRect(0, 0, m_canvas.getWidth(), m_canvas.getHeight());
	}
	
	@Override
	public BufferedImage getImage()
	{
		return SwingFXUtils.fromFXImage(m_image, null);
	}
	
	public Canvas getCanvas()
	{
		return m_canvas;
	}
	
	public int getWidth()
	{
		return (int)(m_image.getWidth());
	}
	
	public int getHeight()
	{
		return (int)(m_image.getHeight());
	}
}
