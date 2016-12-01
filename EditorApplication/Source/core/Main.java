package core;

import java.awt.BorderLayout;
import java.lang.reflect.InvocationTargetException;

import javax.swing.JFrame;
import javax.swing.SwingUtilities;

import photonApi.FloatArrayRef;
import photonApi.FloatRef;
import photonApi.IntRef;
import photonApi.LongRef;
import photonApi.Ph;
import photonApi.PhTest;
import photonCore.FrameData;
import photonCore.PhCamera;
import photonCore.PhFilm;
import photonCore.PhFrame;
import photonCore.PhRenderer;
import photonCore.PhSampleGenerator;
import photonCore.PhUtility;
import photonCore.PhWorld;
import photonCore.exception.PhDataInconsistentException;
import ui.FramePanel;
import ui.Window;

public class Main
{
	private static Window window;
	
	public static void main(String[] args)
	{
		if(!Ph.phStart())
		{
			System.out.println("Photon API initialization failed");
		}
		
		new PhTest();
		
		Ph.printTestMessage();
		
		try
		{
			SwingUtilities.invokeAndWait(new Runnable()
			{
				@Override
				public void run()
				{
					window = new Window(900, 900);
				}
			});
		}
		catch(Exception e)
		{
			e.printStackTrace();
		}
		
		PhCamera camera = new PhCamera(PhCamera.Type.DEFAULT);
		camera.setPosition(0, 0, 16);
		
		PhWorld world = new PhWorld();
		PhRenderer renderer = new PhRenderer(PhRenderer.Type.MT_IMPORTANCE);
		PhFilm film = new PhFilm(900, 900);
		PhSampleGenerator sampleGenerator = new PhSampleGenerator(PhSampleGenerator.Type.STANDARD, 16);
		
		camera.setFilm(film);
		
		renderer.setSampleGenerator(sampleGenerator);
		
		PhUtility.loadTestScene(world);
		world.cook();
		
		Thread queryThread = new Thread((new Runnable()
		{
			@Override
			public void run()
			{
				while(true)
				{
					FloatRef progress = new FloatRef();
					//Ph.phQueryRendererPercentageProgress(rendererId.m_value, progress);
					System.out.println("progress: " + progress.m_value + " %");
					//System.out.println("dassadadadadadad");
					try
					{
						Thread.sleep(3000);
					}
					catch(InterruptedException e)
					{
						e.printStackTrace();
					}
				}
			}
		}));
		queryThread.start();
		
		long t1 = System.currentTimeMillis();
		renderer.render(world, camera);
		long t2 = System.currentTimeMillis();
		System.out.println("time elapsed: " + (double)(t2 - t1) + " ms");
		
		PhFrame frame = new PhFrame(PhFrame.Type.HDR, 900, 900);
		film.develop(frame);
		
		FrameData frameData = new FrameData();
		try
		{
			frame.getData(frameData);
		}
		catch(PhDataInconsistentException e)
		{
			e.printStackTrace();
			System.exit(1);
		}
		
		System.out.println("frame width: " + frameData.getWidthPx() + " | frame height: " + frameData.getHeightPx());
		
		HdrFrame hdrFrame = new HdrFrame(frameData);
		
		
		SwingUtilities.invokeLater(new Runnable()
		{
			@Override
			public void run()
			{
				window.render(hdrFrame);
				System.out.println("rendering done");
			}
		});
	}
}
