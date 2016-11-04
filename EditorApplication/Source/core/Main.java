package core;
import java.awt.BorderLayout;
import java.lang.reflect.InvocationTargetException;

import javax.swing.JFrame;
import javax.swing.SwingUtilities;

import photonApi.FloatArrayRef;
import photonApi.IntRef;
import photonApi.LongRef;
import photonApi.Ph;
import ui.Display;
import ui.Window;

public class Main
{
	private static Window window;
	
	public static void main(String[] args)
	{
		if(!Ph.start())
		{
			System.out.println("Photon API initialization failed");
		}
		

		LongRef frameId = new LongRef();
		frameId.m_value = 999;
		System.out.println(frameId.m_value);
		Ph.createHdrFrame(frameId, 300, 400);
		System.out.println(frameId.m_value);
		Ph.deleteHdrFrame(frameId.m_value);
		
		try
		{
			SwingUtilities.invokeAndWait(new Runnable()
			{
				@Override
				public void run()
				{
					window = new Window(1280, 720);
				}
			});
		}
		catch(Exception e)
		{
			e.printStackTrace();
		}
		
		Ph.printTestMessage();
		
		FloatArrayRef data = new FloatArrayRef();
		IntRef frameWidthPx = new IntRef();
		IntRef frameHeightPx = new IntRef();
		Ph.genTestHdrFrame(data, frameWidthPx, frameHeightPx);
		
		System.out.println("frame width: " + frameWidthPx.m_value + " | frame height: " + frameHeightPx.m_value);
		
		HdrFrame frame = new HdrFrame(frameWidthPx.m_value, frameHeightPx.m_value);
		for(int y = 0; y < frameHeightPx.m_value; y++)
		{
			for(int x = 0; x < frameWidthPx.m_value; x++)
			{
				int baseIndex = (y * frameWidthPx.m_value + x) * 3;
				float r = data.m_value[baseIndex + 0];
				float g = data.m_value[baseIndex + 1];
				float b = data.m_value[baseIndex + 2];
				frame.setPixelRgb(x, y, r, g, b);
			}
		}
		
		SwingUtilities.invokeLater(new Runnable()
		{
			@Override
			public void run()
			{
				window.render(frame);
				System.out.println("rendering done");
			}
		});
	}
}
