package appGui.util;

import java.io.IOException;
import java.net.URL;

import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;

public class UILoader
{
	public UILoader()
	{}
	
	public <T> ViewCtrlPair<T> load(URL fxmlFile)
	{
		try
		{
			FXMLLoader fxmlLoader = new FXMLLoader(fxmlFile);
			return new ViewCtrlPair<T>(fxmlLoader.load(), fxmlLoader.getController());
		}
		catch(IOException e)
		{
			e.printStackTrace();
			new MessagePopup(e);
		}
		
		return new ViewCtrlPair<T>();
	}
	
	public Parent loadView(URL fxmlFile)
	{
		try
		{
			return new FXMLLoader(fxmlFile).load();
		}
		catch(IOException e)
		{
			e.printStackTrace();
			new MessagePopup(e);
		}
		
		return null;
	}
}
