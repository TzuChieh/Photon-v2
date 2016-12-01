package ui;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.image.BufferedImage;

import javax.swing.JPanel;

import core.Color;
import core.HdrFrame;
import core.Vector3f;

@SuppressWarnings("serial")
public class FramePanel extends JPanel
{
	private BufferedImage m_bufferedImage;
	
	public FramePanel()
	{
		super();
		
		//this.setPreferredSize(new Dimension(widthPx, heightPx));
		
		//m_bufferedImage = new BufferedImage(widthPx, heightPx, BufferedImage.TYPE_INT_RGB);
	}
	
	public void render(HdrFrame frame)
	{
		m_bufferedImage = new BufferedImage(frame.getWidthPx(), frame.getHeightPx(), BufferedImage.TYPE_INT_RGB);
		
		Vector3f color = new Vector3f();
		
		for(int x = 0; x < frame.getWidthPx(); x++)
		{
			for(int y = 0; y < frame.getHeightPx(); y++)
			{
				int inversedY = frame.getHeightPx() - y - 1;
				
				float r = frame.getPixelR(x, y);
				float g = frame.getPixelG(x, y);
				float b = frame.getPixelB(x, y);
				
				color.set(r, g, b);
				
				// clamp
//					color.clampLocal(0.0f, 1.0f);
//					r = color.x;
//					g = color.y;
//					b = color.z;
				
				// linear
//					r = Func.clamp(frame.getPixelR(x, y), 0.0f, 1.0f);
//					g = Func.clamp(frame.getPixelG(x, y), 0.0f, 1.0f);
//					b = Func.clamp(frame.getPixelB(x, y), 0.0f, 1.0f);
//					
//					r = (float)Math.pow(r, 1.0 / 2.2);
//					g = (float)Math.pow(g, 1.0 / 2.2);
//					b = (float)Math.pow(b, 1.0 / 2.2);
				
				// Reinhard tone mapping
//					r = r / (1.0f + r);
//					g = g / (1.0f + g);
//					b = b / (1.0f + b);
//					
//					r = (float)Math.pow(r, 1.0 / 2.2);
//					g = (float)Math.pow(g, 1.0 / 2.2);
//					b = (float)Math.pow(b, 1.0 / 2.2);
				
				// Jim Hejl and Richard Burgess-Dawson (GDC)
				// no need of pow(1/2.2)
				color.subLocal(0.004f).clampLocal(0.0f, Float.MAX_VALUE);
				Vector3f numerator = color.mul(6.2f).addLocal(0.5f).mulLocal(color);
				Vector3f denominator = color.mul(6.2f).addLocal(1.7f).mulLocal(color).addLocal(0.06f);
				r = numerator.x / denominator.x;
				g = numerator.y / denominator.y;
				b = numerator.z / denominator.z;
				
				if(r != r || g != g || b != b)
				{
					System.out.println("NaN!");
					//System.exit(1);
				}
				
				m_bufferedImage.setRGB(x, inversedY, Color.toARGBInt(0.0f, r, g, b));
			}
		}
		
		
		
//		if(Input.keyUp(Input.KEY_F1))
//		{
//			try 
//			{
//			    File outputfile = new File(Rand.getFloat0_1() + "_" + Engine.numSamples + "spp.png");
//			    ImageIO.write(m_bufferedImage, "png", outputfile);
//			    
//			    Debug.print("============ IMAGE SAVED ============");
//			} 
//			catch (IOException e) 
//			{
//				Debug.printErr("============ IMAGE SAVING FAILED ============");
//				
//				e.printStackTrace();
//			}
//		}
		
		// safe to call from any thread since repaint() uses EDT internally
		repaint();
	}
	
	@Override
	protected void paintComponent(Graphics graphics)
	{
		super.paintComponent(graphics);
		
		if(m_bufferedImage != null)
		{
			final int panelWidth  = getWidth();
			final int panelHeight = getHeight();
			final float panelAspectRatio = (float)(panelWidth) / (float)(panelHeight);
			final float frameAspectRatio = (float)(m_bufferedImage.getWidth()) / (float)(m_bufferedImage.getHeight());
			
			int imageWidth;
			int imageHeight;
			if(frameAspectRatio > panelAspectRatio)
			{
				imageWidth = panelWidth;
				imageHeight = (int)((float)(imageWidth) / frameAspectRatio);
			}
			else
			{
				imageHeight = panelHeight;
				imageWidth = (int)((float)(imageHeight) * frameAspectRatio);
			}
			
			BufferedImage scaledBufferedImage = new BufferedImage(imageWidth, imageHeight, BufferedImage.TYPE_INT_RGB);
	        Graphics2D g2d = (Graphics2D)scaledBufferedImage.createGraphics();
	        g2d.addRenderingHints(new RenderingHints(RenderingHints.KEY_RENDERING, RenderingHints.VALUE_RENDER_QUALITY));
	        g2d.drawImage(m_bufferedImage, 0, 0, imageWidth, imageHeight, null);
			
			graphics.drawImage(scaledBufferedImage, (panelWidth - imageWidth) / 2, (panelHeight - imageHeight) / 2, null);
		}
	}
}
