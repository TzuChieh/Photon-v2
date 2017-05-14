package appGui;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

import javax.swing.SwingUtilities;

import appModel.EditorApp;
import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;
import photonApi.Ph;
import photonApi.PhTest;
import photonCore.FrameData;
import photonCore.PhFrame;
import photonCore.PhEngine;
import ui.Window;
import ui.task.TaskStatusModel;

public class AppMain extends Application
{
//	private static Window window;
	
	private static final EditorApp editorApp = new EditorApp();
	
	public static void main(String[] args)
	{
		Application.launch(args);
		
		
//		if(Ph.phInit())
//		{
//			System.out.println("Photon initialized");
//		}
//		else
//		{
//			System.err.println("Photon initializing failed");
//		}
//		
//		new PhTest();
//		
//		TaskStatusModel testTaskStatusModel = new TaskStatusModel();
//		TaskStatusModel dummyTaskStatusModel = new TaskStatusModel();
//		
//		try
//		{
//			SwingUtilities.invokeAndWait(new Runnable()
//			{
//				@Override
//				public void run()
//				{
//					window = new Window();
//					window.getTaskPanel().registerTaskStatusModel(testTaskStatusModel);
//					window.getTaskPanel().registerTaskStatusModel(dummyTaskStatusModel);
//				}
//			});
//		}
//		catch(Exception e)
//		{
//			e.printStackTrace();
//		}
//		
//		testTaskStatusModel.setTaskName("test render task");
//		dummyTaskStatusModel.setTaskName("dummy dummy dummy");
//		
//		// for cbox
//		//camera.setPosition(0.000001f, -0.000002f, 16);
////		camera.setPosition(0.000001f, -0.000002f, 3);
//		//camera.setDirection(0.0001f, 0.000002f, -1.0f);
//		
////		camera.setPosition(0.000001f, -0.500002f, 17);
////		camera.setDirection(0.0001f, 0.100002f, -1.0f);
//		
//		// for sponza
////		camera.setPosition(3.5765076f, 2.1717842f, 2.5685565f);
////		camera.setDirection(-0.81385213f, -0.30174536f, -0.49657395f);
//		
//		//ArrayList<PhCamera> bb;
//		
////		final int outputWidth = 1280;
////		final int outputHeight = 720;
////		final int outputWidth = 1400;
////		final int outputHeight = 600;
////		final int outputWidth = 800;
////		final int outputHeight = 800;
////		final int outputWidth = 400;
////		final int outputHeight = 400;
////		final int outputWidth = 150;
////		final int outputHeight = 150;
//		final int numRenderThreads = 4;
//		
//		PhEngine engine = new PhEngine(numRenderThreads);
//		
////		engine.enterCommand("## camera [string type pinhole] [real fov-degree 50] [vector3r position \"0 0 16\"] [vector3r direction \"0 0 -1\"]");
////		engine.enterCommand("## film [integer width 800] [integer height 800]");
////		engine.enterCommand("## sampler [integer spp-budget 16]");
////		engine.enterCommand("## integrator [string type backward-mis]");
////		engine.enterCommand("->");
//		
//		
//		
////		engine.load("../scene/testScene.p2");
////		engine.load("../scene/cbox_simple.p2");
////		engine.load("../scene/cbox_material_test.p2");
//		engine.load("../scene/new_syntax_test.p2");
////		engine.load("../scene/cbox_test.p2"); 
////		engine.load("../scene/exporter_test.p2");
////		engine.load("../scene/LAZIENKA.p2");
////		engine.load("../scene/complex/corridor.p2");
////		engine.load("../scene/complex/corridor_1.p2");
////		engine.load("../scene/complex/FinalAR.p2");
////		engine.load("../scene/complex/FinalAR_1.p2");
////		engine.load("../scene/complex/dvi.p2");
////		engine.load("../scene/complex/rays_chair.p2");
//		
//		Thread queryThread = new Thread((new Runnable()
//		{
//			@Override
//			public void run()
//			{
//				while(true)
//				{
//					float progress = engine.queryPercentageProgress();
//					float frequency = engine.querySampleFrequency();
//					testTaskStatusModel.setPercentageProgress(progress);
//					testTaskStatusModel.setSampleFrequency(frequency);
//					if(progress == 100.0f)
//					{
//						break;
//					}
//					
//					try
//					{
//						Thread.sleep(3000);
//					}
//					catch(InterruptedException e)
//					{
//						e.printStackTrace();
//					}
//				}
//				
//				System.out.println("query thread end");
//			}
//		}));
//		queryThread.start();
//		
//		long t1 = System.currentTimeMillis();
//		engine.render();
//		long t2 = System.currentTimeMillis();
//		System.out.println("time elapsed: " + (double)(t2 - t1) + " ms");
//		
//		PhFrame frame = new PhFrame(PhFrame.Type.HDR);
//		engine.developFilm(frame);
//		
//		FrameData frameData = new FrameData();
//		frame.getData(frameData);
//		
//		System.out.println("frame width: " + frameData.getWidthPx() + " | frame height: " + frameData.getHeightPx());
//		
//		HdrFrame hdrFrame = new HdrFrame(frameData);
//		
//		SwingUtilities.invokeLater(new Runnable()
//		{
//			@Override
//			public void run()
//			{
//				window.getDisplayPanel().render(hdrFrame);
//				System.out.println("rendering done");
//			}
//		});
		
		// TODO: exit Photon on Jframe exit event
//		if(Ph.phExit())
//		{
//			System.out.println("Photon exited");
//		}
//		else
//		{
//			System.err.println("Photon exiting failed");
//		}
	}

	@Override
	public void start(Stage primaryStage) throws Exception
	{
		editorApp.create();
		
		FXMLLoader fxmlLoader = new FXMLLoader(getClass().getResource("AppMain.fxml"));
		
		Parent            mainPane       = fxmlLoader.load();
		AppMainController mainController = fxmlLoader.getController();
		
		mainController.setModel(appMainModel);
		
		primaryStage.setTitle("Photon-v2 version 0.0 | Editor");
		primaryStage.setScene(new Scene(mainPane, 1280,	680));
//		primaryStage.setMaximized(true);
        primaryStage.show();
        
        
		mainController.createNewTask("(default task)");
	}
	
	@Override
	public void stop() throws Exception
	{
		super.stop();
		
		editorApp.decompose();
	}
}
