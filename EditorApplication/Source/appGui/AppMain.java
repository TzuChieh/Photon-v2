package appGui;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.PrintStream;
import java.io.PrintWriter;
import java.net.URI;
import java.nio.file.FileSystem;
import java.nio.file.FileSystems;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.logging.LogManager;
import java.util.zip.GZIPInputStream;

import com.sun.javafx.application.LauncherImpl;

import appModel.EditorApp;
import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;
import jsdl.PinholeCameraCreator;
import jsdl.SDLCommand;
import jsdl.SDLReal;
import jsdl.SDLVector3;
import minecraft.Asset;
import minecraft.EFacing;
import minecraft.FaceReachability;
import minecraft.JSONObject;
import minecraft.LevelData;
import minecraft.LevelMetadata;
import minecraft.MCLogger;
import minecraft.MinecraftInstallation;
import minecraft.MinecraftWorld;
import minecraft.ModelData;
import minecraft.RegionData;
import minecraft.SectionData;
import minecraft.Terrain;
import minecraft.nbt.NBTData;
import minecraft.parser.JSONParser;
import minecraft.parser.LevelParser;
import minecraft.parser.MCAParser;
import minecraft.parser.ModelParser;
import minecraft.parser.NBTParser;
import sun.misc.IOUtils;
import util.SDLConsole;
import util.Vector2f;
import util.Vector3f;

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
		minecraftTest();
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
	
	private static void minecraftTest() throws Exception
	{
//		SectionData.addTransparentBlock("minecraft:dirt");
//		SectionData.addTransparentBlock("minecraft:bedrock");
//		SectionData.addTransparentBlock("minecraft:grass_block");
//		SectionData.addTransparentBlock("minecraft:stone");
//		SectionData.addTransparentBlock("minecraft:stone:1");
//		SectionData.addTransparentBlock("minecraft:stone:2");
//		SectionData.addTransparentBlock("minecraft:stone:3");
//		SectionData.addTransparentBlock("minecraft:stone:4");
//		SectionData.addTransparentBlock("minecraft:stone:5");
//		SectionData.addTransparentBlock("minecraft:stone:6");
//		SectionData.addTransparentBlock("minecraft:grass");
//		SectionData.addTransparentBlock("minecraft:sand");
//		SectionData.addTransparentBlock("minecraft:coal_ore");
//		SectionData.addTransparentBlock("minecraft:iron_ore");
//		SectionData.addTransparentBlock("minecraft:gold_ore");
//		SectionData.addTransparentBlock("minecraft:sandstone");
//		SectionData.addTransparentBlock("minecraft:lava");
//		SectionData.addTransparentBlock("minecraft:cobblestone");
		SectionData.addTransparentBlock("minecraft:water");
		
//		SectionData.addTransparentBlock("minecraft:andesite");
//		SectionData.addTransparentBlock("minecraft:granite");
//		SectionData.addTransparentBlock("minecraft:diorite");
//		SectionData.addTransparentBlock("minecraft:granite");
//		SectionData.addTransparentBlock("minecraft:gravel");
		
		SectionData.addTransparentBlock("minecraft:air");
//		SectionData.addTransparentBlock("minecraft:snow");
		
//		EFacing f = EFacing.UP;
//		FaceReachability reachability = new FaceReachability();
//		System.err.println(reachability);
//		reachability.setReachability(EFacing.DOWN, f, true);
//		System.err.println(reachability);
//		reachability.setReachability(EFacing.DOWN, f, false);
//		System.err.println(reachability);
		
//		MCAParser parser = new MCAParser();
////		RegionData region = parser.parse(new File("./r.1.2.mca"));
//		RegionData region = parser.parse(new File("./r.0.0.mca"));
////		RegionData region = parser.parse(new File("./r.0.-1.mca"));
//		
//		Terrain terrain = new Terrain();
//		terrain.addRegion(region);
//		
//		MinecraftWorld mcWorld = new MinecraftWorld();
//		mcWorld.setTerrain(terrain);
//		
//		mcWorld.setViewpoint(new Vector3f(60, 15, 240));
//		mcWorld.setViewDirection(new Vector3f(1, -0.5f, 3));
////		mcWorld.setViewpoint(new Vector3f(0, 100, 0));
////		mcWorld.setViewDirection(new Vector3f(1, -0.4f, 1));
//		mcWorld.setFovDegrees(70.0f);
//		
//		SDLConsole console = new SDLConsole("mcw_export");
//		console.start();
//		mcWorld.toSDL(console);
//		console.exit();
		
//		ModelParser modelParser = new ModelParser();
//		ModelData modelData = modelParser.parse(new FileInputStream("./birch_log.json"));
//		System.out.println(modelData);
		
		
		MinecraftInstallation mcInstallation = new MinecraftInstallation();
		
		
		System.err.println(mcInstallation.getLevels());
		
		
		
		
		
		Path jarPath = mcInstallation.getJar(13, 2);
        
		try(FileSystem zipfs = FileSystems.newFileSystem(jarPath, null))
		{
			Path modelFolder = zipfs.getPath("assets", "minecraft", "models");
			Path textureFolder = zipfs.getPath("assets", "minecraft", "textures");
			
			Set<String> modelIds = new HashSet<>();
			modelIds.add("block/cobblestone");
			
			Set<String> textureIds = new HashSet<>();
			textureIds.add("block/acacia_log_top");
			textureIds.add("block/acacia_log");
			
			Asset asset = new Asset();
			asset.loadModels(modelFolder, modelIds);
			asset.loadTextures(textureFolder, textureIds);
			
			asset.loadModelTextures(textureFolder);
			
			List<Path> levels = mcInstallation.getLevels();
			LevelData level = new LevelParser().parse(levels.get(0));
			
			System.err.println(level.getMetadata().getSpPlayerPosition());
			System.err.println(level.getMetadata().getSpPlayerYawPitchDegrees());
			
			Terrain terrain = level.getReachableTerrain(level.getMetadata().getSpPlayerPosition());
			
			MinecraftWorld mcWorld = new MinecraftWorld(terrain, asset);
			SDLConsole console = new SDLConsole("mcw_export");
			
			PinholeCameraCreator camera = new PinholeCameraCreator();
			camera.setFovDegree(new SDLReal(105.0f));
			
			Vector3f camPos = level.getMetadata().getSpPlayerPosition();
			camera.setPosition(new SDLVector3(camPos.x, camPos.y, camPos.z));
			
			Vector2f yawPitch = level.getMetadata().getSpPlayerYawPitchDegrees();
			camera.setYawDegrees(new SDLReal(yawPitch.x));
			camera.setPitchDegrees(new SDLReal(yawPitch.y));
			
			console.queue(camera);
			
			
			console.start();
			mcWorld.toSDL(console);
			console.exit();
		}
		
//		System.exit(0);
	}
}
