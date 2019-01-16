package appGui.widget;

import java.io.PrintWriter;
import java.io.StringWriter;

import javafx.scene.control.Alert;
import javafx.scene.control.ScrollPane;
import javafx.scene.control.TextArea;

public final class MessagePopup
{
	public MessagePopup(Exception e)
	{
		StringWriter sw = new StringWriter();
		e.printStackTrace(new PrintWriter(sw));

		Alert alert = new Alert(Alert.AlertType.ERROR);
		alert.setHeaderText("Some Problems Occurred");
		alert.getDialogPane().setExpandableContent(new ScrollPane(new TextArea(sw.toString())));
		alert.showAndWait();
	}
}
