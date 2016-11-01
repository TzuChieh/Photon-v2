package ui;
import java.awt.BorderLayout;

import javax.swing.JFrame;
import javax.swing.SwingUtilities;

import core.HdrFrame;
import photonApi.FloatArrayRef;
import photonApi.IntRef;
import photonApi.Ph;

public class Window
{
	private JFrame m_jframe;
	private Display m_display;
	
	public Window(int widthPx, int heightPx)
	{
		m_jframe = new JFrame("Photon-v2 Editor");
		m_jframe.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		m_jframe.setBounds(0, 0, widthPx, heightPx);
		m_jframe.setLocationRelativeTo(null);
		m_jframe.setLayout(new BorderLayout());
		m_jframe.setResizable(false);
		m_jframe.setVisible(true);
		
		m_display = new Display(widthPx, heightPx);
		m_jframe.add(m_display, BorderLayout.CENTER);
		
		m_jframe.pack();
		m_jframe.revalidate();
		
		FloatArrayRef data = new FloatArrayRef();
		IntRef frameWidthPx = new IntRef();
		IntRef frameHeightPx = new IntRef();
		Ph.genTestHdrFrame(data, frameWidthPx, frameHeightPx);
		
		System.out.println("frame width: " + frameWidthPx.m_value + " | frame height: " + frameHeightPx.m_value);
		
		HdrFrame frame = new HdrFrame(1280, 720);
		
		for(int y = 0; y < 720; y++)
		{
			for(int x = 0; x < 1280; x++)
			{
				float pixelValue = data.m_value[y * 1280 + x];
				frame.setPixelRgb(x, y, pixelValue, pixelValue, pixelValue);
			}
		}
		
		render(frame);
	}
	
	public void render(HdrFrame frame)
	{
		m_display.render(frame);
	}
}
