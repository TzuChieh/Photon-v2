package appModel;

import java.awt.image.BufferedImage;
import java.io.IOException;
import java.nio.file.Path;

import javax.imageio.ImageIO;

import javafx.beans.value.ChangeListener;
import javafx.embed.swing.SwingFXUtils;
import javafx.event.EventHandler;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.image.Image;
import javafx.scene.image.PixelWriter;
import javafx.scene.image.WritableImage;
import javafx.scene.input.MouseEvent;
import javafx.scene.paint.Color;
import photonApi.FrameRegion;
import photonApi.Rectangle;
import util.Vector2f;
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
		
		m_image  = new WritableImage(widthPx, heightPx);
		m_canvas = new Canvas(widthPx, heightPx);
		
		ChangeListener<Number> sizeChangeListener = (ovservable, oldValue, newValue) ->
			getDisplayView().showDisplayResolution((int)(m_canvas.getWidth() + 0.5), (int)(m_canvas.getHeight() + 0.5));
		m_canvas.widthProperty().addListener(sizeChangeListener);
		m_canvas.heightProperty().addListener(sizeChangeListener);
		
		ChangeListener<Number> redrawListener = (ovservable, oldValue, newValue) ->
			{clearFrame(); drawFrame();};
		m_canvas.widthProperty().addListener(redrawListener);
		m_canvas.heightProperty().addListener(redrawListener);
		
		m_canvas.setOnMouseMoved(new EventHandler<MouseEvent>()
		{
			@Override
			public void handle(MouseEvent event)
			{
				Vector2f canvasCoordPx = new Vector2f((float)event.getX(), (float)event.getY());
				Vector2f frameCoordPx  = getFrameCoordPx(canvasCoordPx);
				
				getDisplayView().showCursorCoord(
					Math.min((int)frameCoordPx.x, getFrameWidthPx() - 1), 
					Math.min((int)frameCoordPx.y, getFrameHeightPx() - 1));
				
				int x = (int)frameCoordPx.x;
				int y = (int)(m_image.getHeight() - frameCoordPx.y);
				x = Math.min(x, getFrameWidthPx() - 1);
				y = Math.min(y, getFrameHeightPx() - 1);
				
				Color color = m_image.getPixelReader().getColor(x, y);
				getDisplayView().showCursorColor(
					(int)(color.getRed() * 255.0), 
					(int)(color.getGreen() * 255.0), 
					(int)(color.getBlue() * 255.0));
			}
		});
		
		getDisplayView().showFrameResolution(widthPx, heightPx);
		getDisplayView().showDisplayResolution(widthPx, heightPx);
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
			
			getDisplayView().showFrameResolution(frameRegion.getFullWidthPx(), frameRegion.getFullHeightPx());
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
		Vector2f drawResPx = getFittedDrawResPx();
		Vector2f originPx  = getCenteredOriginPx(drawResPx);
		
		GraphicsContext g = m_canvas.getGraphicsContext2D();
		g.drawImage(
			m_image, 
			originPx.x, (m_canvas.getHeight() - originPx.y) - drawResPx.y, 
			drawResPx.x, drawResPx.y);
		
		getDisplayView().showZoom(getDrawnScale(drawResPx) * 100.0f);
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
	
	public int getFrameWidthPx()
	{
		return (int)(m_image.getWidth());
	}
	
	public int getFrameHeightPx()
	{
		return (int)(m_image.getHeight());
	}
	
	private Vector2f getFittedDrawResPx()
	{
		final float canvasWidth       = (float)(m_canvas.getWidth());
		final float canvasHeight      = (float)(m_canvas.getHeight());
		final float canvasAspectRatio = canvasWidth / canvasHeight;
		final float frameAspectRatio  = (float)(m_image.getWidth()) / (float)(m_image.getHeight());
		
		Vector2f resPx = new Vector2f();
		if(frameAspectRatio > canvasAspectRatio)
		{
			resPx.x  = (int)canvasWidth;
			resPx.y = (int)(canvasWidth / frameAspectRatio);
		}
		else
		{
			resPx.x = (int)(canvasHeight * frameAspectRatio);
			resPx.y = (int)canvasHeight;
		}
		
		return resPx;
	}
	
	private Vector2f getCenteredOriginPx(Vector2f drawSizePx)
	{
		return new Vector2f(
			(float)(m_canvas.getWidth() - drawSizePx.x) * 0.5f, 
			(float)(m_canvas.getHeight() - drawSizePx.y) * 0.5f);
	}
	
	private Vector2f getFrameCoordPx(Vector2f canvasCoordPx)
	{
		// FIXME: centered and fitted frame assumed
		Vector2f drawResPx = getFittedDrawResPx();
		Vector2f originPx  = getCenteredOriginPx(drawResPx);
		
		// flipping canvas y
		Vector2f coordPx = new Vector2f(canvasCoordPx.x, (float)m_canvas.getHeight() - canvasCoordPx.y);
		
		// relative to drawed frame
		coordPx = coordPx.sub(originPx);
		
		// normalize and scale to match actual frame size
		Vector2f frameSizePx = new Vector2f((float)m_image.getWidth(), (float)m_image.getHeight());
		coordPx = coordPx.div(drawResPx).mul(frameSizePx);
		
		// clamp to edge
		coordPx.x = Math.max(Math.min(coordPx.x, frameSizePx.x), 0);
		coordPx.y = Math.max(Math.min(coordPx.y, frameSizePx.y), 0);
		
		return coordPx;
	}
	
	private float getDrawnScale(Vector2f drawSizePx)
	{
		// FIXME: retained aspect ratio assumed
		return (float)(drawSizePx.x / m_image.getWidth());
	}
}
