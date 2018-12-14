package appGui;

import javafx.animation.KeyFrame;
import javafx.animation.KeyValue;
import javafx.animation.Timeline;
import javafx.application.Preloader;
import javafx.application.Preloader.StateChangeNotification.Type;
import javafx.geometry.Rectangle2D;
import javafx.scene.Scene;
import javafx.scene.control.Label;
import javafx.scene.control.ProgressBar;
import javafx.scene.image.Image;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.Background;
import javafx.scene.layout.BackgroundImage;
import javafx.scene.layout.BackgroundPosition;
import javafx.scene.layout.BackgroundRepeat;
import javafx.scene.layout.BackgroundSize;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.Region;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.stage.Screen;
import javafx.stage.Stage;
import javafx.stage.StageStyle;
import javafx.util.Duration;

public class AppPreloader extends Preloader
{
	private Stage m_preloaderStage;
	
	@Override
	public void start(Stage primaryStage) throws Exception
	{
		m_preloaderStage = primaryStage;
		
		double logoWidthPx  = 540;
		double logoHeightPx = 264;
		
		BackgroundImage logo = new BackgroundImage(
			new Image(getClass().getResource("/images/logo_big.png").toString(), logoWidthPx, logoHeightPx, false, true),
			BackgroundRepeat.NO_REPEAT, BackgroundRepeat.NO_REPEAT, 
			BackgroundPosition.DEFAULT, BackgroundSize.DEFAULT);
 
		AnchorPane root = new AnchorPane();
		root.setBackground(new Background(logo));
		
		Scene scene = new Scene(root, logoWidthPx, logoHeightPx, Color.BLACK);
		scene.setFill(Color.TRANSPARENT);
		
		// center the stage
		Rectangle2D screenBounds = Screen.getPrimary().getVisualBounds();
		primaryStage.setX((screenBounds.getWidth() - logoWidthPx) / 2);
		primaryStage.setY((screenBounds.getHeight() - logoHeightPx) / 2);
		
		primaryStage.setTitle("|   LOADING...  | Engine will start shortly...");
		primaryStage.setScene(scene);
		primaryStage.initStyle(StageStyle.TRANSPARENT);
		primaryStage.setAlwaysOnTop(true);
		primaryStage.show();
	}
	
	@Override
	public void handleStateChangeNotification(StateChangeNotification stateChangeNotification)
	{
		if(stateChangeNotification.getType() == Type.BEFORE_START)
		{
			// fully opaque for 1 second
            KeyFrame keyFrame1 = new KeyFrame(
            	Duration.millis(1000),
            	new KeyValue(m_preloaderStage.getScene().getRoot().opacityProperty(), 1));
            
            // then linearly fade out in 1 second
            KeyFrame keyFrame2 = new KeyFrame(
            	Duration.millis(2000),
            	new KeyValue(m_preloaderStage.getScene().getRoot().opacityProperty(), 0));
            
			Timeline timeline = new Timeline();
			timeline.getKeyFrames().add(keyFrame1);
			timeline.getKeyFrames().add(keyFrame2);
			timeline.setOnFinished((ae) -> m_preloaderStage.hide());
			timeline.play();
		}
	}
}
