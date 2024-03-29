package appModel;

import java.awt.image.BufferedImage;

import javafx.beans.value.ChangeListener;
import javafx.embed.swing.SwingFXUtils;
import javafx.event.EventHandler;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.image.PixelWriter;
import javafx.scene.image.WritableImage;
import javafx.scene.input.MouseEvent;
import javafx.scene.paint.Color;
import photonApi.FrameRegion;
import photonApi.Rectangle;
import util.AABB2D;
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
		{
			drawFlood();
			drawFrame(new Rectangle(0, 0, getFrameWidthPx(), getFrameHeightPx()));
			
			getDisplayInfoView().showDisplayResolution(
				(int)(m_canvas.getWidth() + 0.5), (int)(m_canvas.getHeight() + 0.5));
			getDisplayInfoView().showZoom(getDrawnScale(getFittedDrawResPx()) * 100.0f);
		};
			
		m_canvas.widthProperty().addListener(sizeChangeListener);
		m_canvas.heightProperty().addListener(sizeChangeListener);
		
//		m_canvas.widthProperty().addListener((ovservable, oldValue, newValue) -> m_canvas.widthProperty().set(Math.floor(m_canvas.getWidth())));
//		m_canvas.heightProperty().addListener((ovservable, oldValue, newValue) -> m_canvas.heightProperty().set(Math.floor(m_canvas.getHeight())));
		
//		ChangeListener<Number> redrawListener = (ovservable, oldValue, newValue) ->
//			{clear(); drawFrame()};
//		m_canvas.widthProperty().addListener(redrawListener);
//		m_canvas.heightProperty().addListener(redrawListener);
		
		m_canvas.setOnMouseMoved(new EventHandler<MouseEvent>()
		{
			@Override
			public void handle(MouseEvent event)
			{
				Vector2f canvasCoordPx = new Vector2f((float)event.getX(), (float)event.getY());
				Vector2f frameCoordPx  = getFrameCoordPx(canvasCoordPx);
				
				getDisplayInfoView().showCursorCoord(
					Math.min((int)frameCoordPx.x, getFrameWidthPx() - 1), 
					Math.min((int)frameCoordPx.y, getFrameHeightPx() - 1));
				
				int x = (int)frameCoordPx.x;
				int y = (int)(m_image.getHeight() - frameCoordPx.y);
				x = Math.min(x, getFrameWidthPx() - 1);
				y = Math.min(y, getFrameHeightPx() - 1);
				
				Color color = m_image.getPixelReader().getColor(x, y);
				getDisplayInfoView().showCursorColor(
					(int)(color.getRed() * 255.0), 
					(int)(color.getGreen() * 255.0), 
					(int)(color.getBlue() * 255.0));
			}
		});
		
		getDisplayInfoView().showFrameResolution(widthPx, heightPx);
		getDisplayInfoView().showDisplayResolution(widthPx, heightPx);
	}
	
	@Override
	public void loadFrame(FrameRegion frame)
	{
		copyFrameToImage(frame);
	}
	
	@Override
	public void drawFrame(Rectangle region)
	{
		// XXX: Slightly increase the region that is drawn onto canvas, so that
		// seams caused by filtering of original image and background color can
		// be overwrite. This hack causes indicators missing some edges.
		region.x -= 1;
		region.y -= 1;
		region.w += 2;
		region.h += 2;
		
		AABB2D rect = toCanvasRegion(region);
		
		final GraphicsContext g = m_canvas.getGraphicsContext2D();
		g.drawImage(m_image,
			region.x, getFrameHeightPx() - (region.y + region.h), 
			region.w, region.h,
			rect.min.x, rect.min.y, 
			rect.getWidth(), rect.getHeight());
	}
	
	@Override
	public void drawIndicator(Rectangle region, int r255, int g255, int b255)
	{
		final GraphicsContext g = m_canvas.getGraphicsContext2D();
		g.setFill(new Color(r255 / 255.0, g255 / 255.0, b255 / 255.0, 1));
		
		AABB2D rect = toCanvasRegion(region);
		
		float verticalWidth = Math.min(2, rect.getHeight());
		float horizontalWidth = Math.min(2, rect.getWidth());
		
		g.fillRect(rect.min.x, rect.min.y, rect.getWidth(), verticalWidth);
		g.fillRect(rect.min.x, rect.max.y - verticalWidth, rect.getWidth(), verticalWidth);
		g.fillRect(rect.min.x, rect.min.y, horizontalWidth, rect.getHeight());
		g.fillRect(rect.max.x - horizontalWidth, rect.min.y, horizontalWidth, rect.getHeight());
	}
	
	@Override
	public void drawFlood()
	{
		GraphicsContext g = m_canvas.getGraphicsContext2D();
		g.setFill(Color.DARKBLUE);
//		g.setFill(Color.TRANSPARENT);
		g.fillRect(0, 0, m_canvas.getWidth(), m_canvas.getHeight());
	}
	
	@Override
	public BufferedImage getImage()
	{
		return SwingFXUtils.fromFXImage(m_image, null);
	}
	
	public Canvas getView()
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
			resPx.x = canvasWidth;
			resPx.y = canvasWidth / frameAspectRatio;
		}
		else
		{
			resPx.x = canvasHeight * frameAspectRatio;
			resPx.y = canvasHeight;
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
	
	private AABB2D toCanvasRegion(Rectangle region)
	{
		// FIXME: centered and fitted frame assumed
		Vector2f drawResPx = getFittedDrawResPx();
		Vector2f originPx  = getCenteredOriginPx(drawResPx);
		
		AABB2D canvasRegion = new AABB2D(
			new Vector2f(
					originPx.x + ((float)region.x / getFrameWidthPx()) * drawResPx.x, 
					originPx.y + (1.0f - (float)(region.y + region.h) / getFrameHeightPx()) * drawResPx.y),
			new Vector2f(
				originPx.x + ((float)(region.x + region.w) / getFrameWidthPx()) * drawResPx.x, 
				originPx.y + (1.0f - (float)(region.y) / getFrameHeightPx()) * drawResPx.y));
		return canvasRegion;
	}
	
	private float getDrawnScale(Vector2f drawSizePx)
	{
		// FIXME: retained aspect ratio assumed
		return (float)(drawSizePx.x / m_image.getWidth());
	}
	
	private void copyFrameToImage(FrameRegion frame)
	{
		if(!frame.isValid() || frame.getNumComp() != 3)
		{
			System.err.println("unexpected frame format; unable to load");
			return;
		}
		
		if(m_image.getWidth()  != frame.getFullWidthPx() || 
		   m_image.getHeight() != frame.getFullHeightPx())
		{
			m_image = new WritableImage(frame.getFullWidthPx(), frame.getFullHeightPx());
			
//			for(int y = 0; y < m_image.getHeight(); ++y)
//			{
//				for(int x = 0; x < m_image.getWidth(); ++x)
//				{
//					m_image.getPixelWriter().setColor(x, y, new Color(0, 0, 0, 1));
//				}
//			}
			
			getDisplayInfoView().showFrameResolution(frame.getFullWidthPx(), frame.getFullHeightPx());
			getDisplayInfoView().showZoom(getDrawnScale(getFittedDrawResPx()) * 100.0f);
		}
		
		final PixelWriter pixelWriter = m_image.getPixelWriter();
//		final PixelWriter pixelWriter = m_canvas.getGraphicsContext2D().getPixelWriter();
		final Rectangle region = frame.getRegion();
		final int endX = region.x + region.w;
		final int endY = region.y + region.h;
		final Vector3f color = new Vector3f();
		
		for(int y = region.y; y < endY; ++y)
		{
			for(int x = region.x; x < endX; ++x)
			{
				color.set(frame.getRgb(x, y));
				if(!Float.isFinite(color.x) ||
				   !Float.isFinite(color.y) || 
				   !Float.isFinite(color.z))
				{
					System.err.println("color is not finite: " + color);
					color.set(0, 0, 0);
				}
				
				color.clampLocal(0.0f, 1.0f);
				
				int inversedY = frame.getFullHeightPx() - y - 1;
				Color fxColor = new Color(color.x, color.y, color.z, 1.0);
				pixelWriter.setColor(x, inversedY, fxColor);
			}
		}
	}
}
