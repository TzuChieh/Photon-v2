package ui;
import java.awt.Dimension;
import java.awt.image.BufferedImage;

import javax.swing.JPanel;

public class Display extends JPanel
{
	private BufferedImage m_bufferedImage;
	
	public Display(int widthPx, int heightPx)
	{
		super();
		
		this.setPreferredSize(new Dimension(widthPx, heightPx));
		
		m_bufferedImage = new BufferedImage(widthPx, heightPx, BufferedImage.TYPE_INT_RGB);
	}
	
	// safe to call from any thread since repaint() uses EDT internally
//	public void render(HdrFrame frame)
//	{
//		synchronized(m_bufferedImage)
//		{
//			Vector3f color = new Vector3f();
//			
//			for(int x = 0; x < frame.getWidthPx(); x++)
//			{
//				for(int y = 0; y < frame.getHeightPx(); y++)
//				{
//					int inversedY = frame.getHeightPx() - y - 1;
//					
//					float r = frame.getPixelR(x, y);
//					float g = frame.getPixelG(x, y);
//					float b = frame.getPixelB(x, y);
//					
//					color.set(r, g, b);
//					
//					
//					// linear
////					r = Func.clamp(frame.getPixelR(x, y), 0.0f, 1.0f);
////					g = Func.clamp(frame.getPixelG(x, y), 0.0f, 1.0f);
////					b = Func.clamp(frame.getPixelB(x, y), 0.0f, 1.0f);
////					
////					r = (float)Math.pow(r, 1.0 / 2.2);
////					g = (float)Math.pow(g, 1.0 / 2.2);
////					b = (float)Math.pow(b, 1.0 / 2.2);
//					
//					// Reinhard tone mapping
////					r = r / (1.0f + r);
////					g = g / (1.0f + g);
////					b = b / (1.0f + b);
////					
////					r = (float)Math.pow(r, 1.0 / 2.2);
////					g = (float)Math.pow(g, 1.0 / 2.2);
////					b = (float)Math.pow(b, 1.0 / 2.2);
//					
//					// Jim Hejl and Richard Burgess-Dawson (GDC)
//					// no need of pow(1/2.2)
//					color.subLocal(0.004f).clampLocal(0.0f, Float.MAX_VALUE);
//					Vector3f numerator = color.mul(6.2f).addLocal(0.5f).mulLocal(color);
//					Vector3f denominator = color.mul(6.2f).addLocal(1.7f).mulLocal(color).addLocal(0.06f);
//					r = numerator.x / denominator.x;
//					g = numerator.y / denominator.y;
//					b = numerator.z / denominator.z;
//					
//					if(r != r || g != g || b != b)
//					{
//						System.out.println("NaN!");
//						System.exit(1);
//					}
//					
//					m_bufferedImage.setRGB(x, inversedY, Color.toARGBInt(0.0f, r, g, b));
//				}
//			}
//		}
//		
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
//		
//		this.repaint();
//	}
//	
//	@Override
//	protected void paintComponent(Graphics graphics)
//	{
//		super.paintComponent(graphics);
//		
//		synchronized(m_bufferedImage)
//		{
//			graphics.drawImage(m_bufferedImage, 0, 0, null);
//		}
//	}
}
