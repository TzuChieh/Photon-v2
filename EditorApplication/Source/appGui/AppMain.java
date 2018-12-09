package appGui;

import java.io.PrintStream;

import com.sun.javafx.application.LauncherImpl;

import appModel.EditorApp;
import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;

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
