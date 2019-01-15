package appGui;

import javafx.fxml.FXML;
import javafx.scene.web.WebView;

public class ScenesCtrl
{
	@FXML private WebView webView;
	
	@FXML
	public void initialize()
	{
		webView.getEngine().load("https://tzuchieh.github.io/download_scenes_metallic_chess.html");
	}
}
