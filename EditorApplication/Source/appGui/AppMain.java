package appGui;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintStream;

import javax.swing.SwingUtilities;

import appModel.EditorApp;
import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;
import photonApi.FrameData;
import photonApi.Ph;
import photonApi.PhEngine;
import photonApi.PhFrame;
import photonApi.PhTest;
import ui.Window;
import ui.task.TaskStatusModel;

public class AppMain extends Application
{
	private static final EditorApp editorApp = new EditorApp();
	
	private static final OutputStream consoleOutputStream = new OutputStream()
	{
		private StringBuilder msgBuilder = new StringBuilder();
		
		@Override
		public void write(int b) throws IOException
		{
			char ch = (char)b;
			if(ch != '\n')
			{
				msgBuilder.append(ch);
			}
			else
			{
				EditorApp.printToConsole(msgBuilder.toString());
				msgBuilder.setLength(0);
			}
		}
	};
	
	private static final PrintStream originalOut = System.out;
	private static final PrintStream originalErr = System.err;
	
	public static void main(String[] args)
	{
		Application.launch(args);
	}

	@Override
	public void start(Stage primaryStage) throws Exception
	{
		editorApp.create();
		
//		System.setOut(new PrintStream(consoleOutputStream, true));
//		System.setErr(new PrintStream(consoleOutputStream, true));
		
		FXMLLoader fxmlLoader = new FXMLLoader(getClass().getResource("AppMain.fxml"));
		
		Parent      appMainView = fxmlLoader.load();
		AppMainCtrl appMainCtrl = fxmlLoader.getController();
		
		appMainCtrl.setEditorApp(editorApp);
		appMainCtrl.createNewProject("(default project)");
		
		primaryStage.setTitle("Photon-v2 version 0.0 | Editor");
		primaryStage.setScene(new Scene(appMainView, 1280,	680));
//		primaryStage.setMaximized(true);
        primaryStage.show();
	}
	
	@Override
	public void stop() throws Exception
	{
		System.setOut(originalOut);
		System.setErr(originalErr);
		
		editorApp.decompose();
	}
}
