package appGui;

import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.control.Accordion;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.TitledPane;
import javafx.scene.layout.BorderPane;

public class MinecraftCtrl
{
	@FXML private BorderPane mainPane;
	@FXML private Accordion  sideAccordion;
	@FXML private TitledPane loadMapPane;
	@FXML private TitledPane assetPane;
	@FXML private TitledPane exportPane;
	@FXML private Label      mapNameLabel;
	
	@FXML
	public void initialize()
	{
		sideAccordion.setExpandedPane(loadMapPane);
		mapNameLabel.setText("(empty)");
	}
	
	@FXML
	void fromSavedGamesButtonClicked(ActionEvent event)
	{
		// TODO
	}
	
	@FXML
	void manuallySelectButtonClicked(ActionEvent event)
	{
		// TODO
	}
}
