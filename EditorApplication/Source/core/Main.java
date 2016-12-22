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
import ui.Window;
import ui.display.ImagePanel;
import ui.model.TaskStatusModel;

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
		
		TaskStatusModel testTaskStatusModel = new TaskStatusModel();
		TaskStatusModel dummyTaskStatusModel = new TaskStatusModel();
		
		try
		{
			SwingUtilities.invokeAndWait(new Runnable()
			{
				@Override
				public void run()
				{
					window = new Window();
					window.getTaskPanel().registerTaskStatusModel(testTaskStatusModel);
					window.getTaskPanel().registerTaskStatusModel(dummyTaskStatusModel);
				}
			});
		}
		catch(Exception e)
		{
			e.printStackTrace();
		}
		
		testTaskStatusModel.setTaskName("test render task");
		dummyTaskStatusModel.setTaskName("dummy dummy dummy");
		
		PhCamera camera = new PhCamera(PhCamera.Type.DEFAULT);
		camera.setPosition(0, 0, 16);
		
		// for sponza
//		camera.setPosition(3.5765076f, 2.1717842f, 2.5685565f);
//		camera.setDirection(-0.81385213f, -0.30174536f, -0.49657395f);
		
		
//		final int outputWidth = 1280;
//		final int outputHeight = 720;
		final int outputWidth = 900;
		final int outputHeight = 900;
		final int numRenderThreads = 4;
		
		
		PhWorld world = new PhWorld();
		
		PhSampleGenerator sampleGenerator = new PhSampleGenerator(PhSampleGenerator.Type.PIXEL_JITTER, 16);
		PhRenderer renderer = new PhRenderer(PhRenderer.Type.MT_IMPORTANCE, numRenderThreads);
		renderer.setSampleGenerator(sampleGenerator);
		
//		PhRenderer renderer = new PhRenderer(PhRenderer.Type.PREVIEW, numRenderThreads);
		PhFilm film = new PhFilm(outputWidth, outputHeight);
		
		
		camera.setFilm(film);
		
		
		
		PhUtility.loadTestScene(world);
		world.cook();
		
		Thread queryThread = new Thread((new Runnable()
		{
			@Override
			public void run()
			{
				while(true)
				{
					float progress = renderer.queryPercentageProgress();
					float frequency = renderer.querySampleFrequency();
					testTaskStatusModel.setPercentageProgress(progress);
					testTaskStatusModel.setSampleFrequency(frequency);
					if(progress == 100.0f)
					{
						break;
					}
					
					try
					{
						Thread.sleep(3000);
					}
					catch(InterruptedException e)
					{
						e.printStackTrace();
					}
				}
				
				System.out.println("query thread end");
			}
		}));
		queryThread.start();
		
		long t1 = System.currentTimeMillis();
		renderer.render(world, camera);
		long t2 = System.currentTimeMillis();
		System.out.println("time elapsed: " + (double)(t2 - t1) + " ms");
		
		PhFrame frame = new PhFrame(PhFrame.Type.HDR, outputWidth, outputHeight);
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
				window.getDisplayPanel().render(hdrFrame);
				System.out.println("rendering done");
			}
		});
	}
}
