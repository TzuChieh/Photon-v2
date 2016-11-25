package core;

import java.awt.BorderLayout;
import java.lang.reflect.InvocationTargetException;

import javax.swing.JFrame;
import javax.swing.SwingUtilities;

import photonApi.FloatArrayRef;
import photonApi.IntRef;
import photonApi.LongRef;
import photonApi.Ph;
import photonApi.PhTest;
import ui.Display;
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

//		LongRef frameId = new LongRef();
//		frameId.m_value = 999;
//		System.out.println(frameId.m_value);
//		Ph.phCreateFrame(frameId, 300, 400, Ph.PH_HDR_FRAME_TYPE);
//		System.out.println(frameId.m_value);
//		Ph.phDeleteFrame(frameId.m_value);
		
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
		
		FloatArrayRef data = new FloatArrayRef();
		IntRef frameWidthPx = new IntRef();
		IntRef frameHeightPx = new IntRef();
		Ph.genTestHdrFrame(data, frameWidthPx, frameHeightPx);
		
		System.out.println("frame width: " + frameWidthPx.m_value + " | frame height: " + frameHeightPx.m_value);
		
		HdrFrame hdrFrame = new HdrFrame(frameWidthPx.m_value, frameHeightPx.m_value);
		for(int y = 0; y < frameHeightPx.m_value; y++)
		{
			for(int x = 0; x < frameWidthPx.m_value; x++)
			{
				int baseIndex = (y * frameWidthPx.m_value + x) * 3;
				float r = data.m_value[baseIndex + 0];
				float g = data.m_value[baseIndex + 1];
				float b = data.m_value[baseIndex + 2];
				hdrFrame.setPixelRgb(x, y, r, g, b);
			}
		}
		
//		LongRef cameraId = new LongRef();
//		Ph.phCreateCamera(cameraId, Ph.PH_DEFAULT_CAMERA_TYPE);
//		Ph.phSetCameraPosition(cameraId.m_value, 0, 0, 4);
//		
//		LongRef worldId = new LongRef();
//		Ph.phCreateWorld(worldId);
//		
//		LongRef rendererId = new LongRef();
////		Ph.phCreateRenderer(rendererId, Ph.PH_IMPORTANCE_RENDERER_TYPE);
//		Ph.phCreateRenderer(rendererId, Ph.PH_MT_IMPORTANCE_RENDERER_TYPE);
//		
//		LongRef filmId = new LongRef();
//		Ph.phCreateFilm(filmId, 1280, 720);
//		
//		LongRef sampleGeneratorId = new LongRef();
//		Ph.phCreateSampleGenerator(sampleGeneratorId, Ph.PH_STANDARD_SAMPLE_GENERATOR_TYPE, 16);
//		
//		Ph.phSetCameraFilm(cameraId.m_value, filmId.m_value);
//		Ph.phSetRendererSampleGenerator(rendererId.m_value, sampleGeneratorId.m_value);
//		Ph.phLoadTestScene(worldId.m_value);
//		Ph.phCookWorld(worldId.m_value);
//		
//		Ph.phRender(rendererId.m_value, worldId.m_value, cameraId.m_value);
//		
//		LongRef frameId = new LongRef();
//		Ph.phCreateFrame(frameId, Ph.PH_HDR_FRAME_TYPE, 1280, 720);
//		Ph.phDevelopFilm(filmId.m_value, frameId.m_value);
//		
//		FloatArrayRef pixelData = new FloatArrayRef();
//		IntRef widthPx = new IntRef();
//		IntRef heightPx = new IntRef();
//		IntRef nPixelComponents = new IntRef();
//		Ph.phGetFrameData(frameId.m_value, pixelData, widthPx, heightPx, nPixelComponents);
//		
//		System.out.println("frame width: " + widthPx.m_value + " | frame height: " + heightPx.m_value);
//		
//		HdrFrame hdrFrame = new HdrFrame(widthPx.m_value, heightPx.m_value);
//		for(int y = 0; y < heightPx.m_value; y++)
//		{
//			for(int x = 0; x < widthPx.m_value; x++)
//			{
//				int baseIndex = (y * widthPx.m_value + x) * 3;
//				float r = pixelData.m_value[baseIndex + 0];
//				float g = pixelData.m_value[baseIndex + 1];
//				float b = pixelData.m_value[baseIndex + 2];
//				hdrFrame.setPixelRgb(x, y, r, g, b);
//			}
//		}
		
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
