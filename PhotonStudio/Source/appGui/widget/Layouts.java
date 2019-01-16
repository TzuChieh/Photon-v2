package appGui.widget;

import javafx.scene.Parent;
import javafx.scene.layout.AnchorPane;

public final class Layouts
{
	public static void addAnchored(AnchorPane parent, Parent child)
	{
		parent.getChildren().add(child);
    	
    	AnchorPane.setTopAnchor(child, 0.0);
    	AnchorPane.setBottomAnchor(child, 0.0);
    	AnchorPane.setLeftAnchor(child, 0.0);
    	AnchorPane.setRightAnchor(child, 0.0);
	}
}
