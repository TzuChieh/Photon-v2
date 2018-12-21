package appGui;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.PrintStream;
import java.io.PrintWriter;
import java.util.List;
import java.util.zip.GZIPInputStream;

import com.sun.javafx.application.LauncherImpl;

import appModel.EditorApp;
import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;
import jsdl.SDLCommand;
import jsdl.SDLVector3;
import sun.misc.IOUtils;
import util.SDLConsole;
import util.Vector3f;
import util.minecraft.EFacing;
import util.minecraft.FaceReachability;
import util.minecraft.JSONObject;
import util.minecraft.MinecraftWorld;
import util.minecraft.RegionData;
import util.minecraft.Terrain;
import util.minecraft.nbt.NBTData;
import util.minecraft.parser.JSONParser;
import util.minecraft.parser.MCAParser;
import util.minecraft.parser.NBTParser;

public class AppMain extends Application
{
	private EditorApp m_editorApp;
	
//	private static final OutputStream consoleOutputStream = new OutputStream()
//	{
//		private StringBuilder msgBuilder = new StringBuilder();
//		
//		@Override
//		public void write(int b) throws IOException
//		{
//			char ch = (char)b;
//			if(ch != '\n')
//			{
//				msgBuilder.append(ch);
//			}
//			else
//			{
//				EditorApp.printToConsole(msgBuilder.toString());
//				msgBuilder.setLength(0);
//			}
//		}
//	};
	
	private static final PrintStream originalOut = System.out;
	private static final PrintStream originalErr = System.err;
	
	public static void main(String[] args)
	{
		//Application.launch(args);
		
		// FIXME: use the standard way
		LauncherImpl.launchApplication(AppMain.class, AppPreloader.class, args);
	}
	
	@Override
	public void init() throws Exception
	{
		m_editorApp = new EditorApp();
		
		// DEBUG
		
//		EFacing f = EFacing.UP;
//		FaceReachability reachability = new FaceReachability();
//		System.err.println(reachability);
//		reachability.setReachability(EFacing.DOWN, f, true);
//		System.err.println(reachability);
//		reachability.setReachability(EFacing.DOWN, f, false);
//		System.err.println(reachability);
		
		MCAParser parser = new MCAParser();
//		RegionData region = parser.parse(new File("./r.1.2.mca"));
		RegionData region = parser.parse(new File("./r.0.0.mca"));
//		RegionData region = parser.parse(new File("./r.0.-1.mca"));
		
		Terrain terrain = new Terrain();
		terrain.addRegion(region);
		
		MinecraftWorld mcWorld = new MinecraftWorld();
		mcWorld.setTerrain(terrain);
		
//		mcWorld.setViewpoint(new Vector3f(60, 15, 240));
		mcWorld.setViewpoint(new Vector3f(0, 100, 0));
//		mcWorld.setViewDirection(new Vector3f(1, -0.5f, 3));
		mcWorld.setViewDirection(new Vector3f(1, -0.4f, 1));
		mcWorld.setFovDegrees(70.0f);
		
		SDLConsole console = new SDLConsole("mcw_export");
		console.start();
		mcWorld.toSDL(console);
		console.exit();
		
//		System.exit(0);
	}

	@Override
	public void start(Stage primaryStage) throws Exception
	{
		m_editorApp.create();
		
//		System.setOut(new PrintStream(consoleOutputStream, true));
//		System.setErr(new PrintStream(consoleOutputStream, true));
		
		FXMLLoader fxmlLoader = new FXMLLoader(getClass().getResource("/fxmls/AppMain.fxml"));
		
		Parent      appMainView = fxmlLoader.load();
		AppMainCtrl appMainCtrl = fxmlLoader.getController();
		
		appMainCtrl.setEditorApp(m_editorApp);
		appMainCtrl.createNewProject("(default project)");
		appMainCtrl.setWorkbenchAsEditorView();
		
		Scene scene = new Scene(appMainView, 1280, 680);
		
		// Overrides this attribute to a dark color to get a dark theme.
		// (many colors in default .css are dependent on this attribute)
		//
		scene.getRoot().setStyle("-fx-base: rgba(25, 25, 25, 255);");
		
		primaryStage.setTitle("Photon-v2 version 0.0 | Editor");
		primaryStage.setScene(scene);
//		primaryStage.setMaximized(true);
        primaryStage.show();
	}
	
	@Override
	public void stop() throws Exception
	{
		System.setOut(originalOut);
		System.setErr(originalErr);
		
		m_editorApp.decompose();
	}
}
