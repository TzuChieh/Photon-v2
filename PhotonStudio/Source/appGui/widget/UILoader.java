package appGui.widget;

import java.io.IOException;
import java.net.URL;

import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;

public class UILoader
{
	public UILoader()
	{}
	
	public <T> UI<T> load(URL fxmlFile)
	{
		try
		{
			FXMLLoader fxmlLoader = new FXMLLoader(fxmlFile);
			return new UI<T>(fxmlLoader.load(), fxmlLoader.getController());
		}
		catch(IOException e)
		{
			e.printStackTrace();
			new MessagePopup(e);
		}
		
		return new UI<T>();
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
