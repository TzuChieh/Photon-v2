package appGui;

import appModel.DisplayInfoView;
import javafx.fxml.FXML;
import javafx.scene.control.Label;
import javafx.scene.paint.Color;
import javafx.scene.shape.Rectangle;

public class DisplayInfoCtrl
{
	@FXML private Label     frameResolutionX;
	@FXML private Label     frameResolutionY;
	@FXML private Label     cursorX;
	@FXML private Label     cursorY;
	@FXML private Label     cursorR;
	@FXML private Label     cursorG;
	@FXML private Label     cursorB;
	@FXML private Label     percentageZoom;
	@FXML private Rectangle colorRectangle;
	@FXML private Label     displayResolutionX;
	@FXML private Label     displayResolutionY;
	
	private DisplayInfoView m_view;
	
	@FXML
    public void initialize()
	{
		m_view = new DisplayInfoView()
		{
			@Override
			public void showFrameResolution(int widthPx, int heightPx)
			{
				frameResolutionX.setText(Integer.toString(widthPx));
				frameResolutionY.setText(Integer.toString(heightPx));
			}
			
			@Override
			public void showDisplayResolution(int widthPx, int heightPx)
			{
				displayResolutionX.setText(Integer.toString(widthPx));
				displayResolutionY.setText(Integer.toString(heightPx));
			}
			
			@Override
			public void showCursorCoord(int xPx, int yPx)
			{
				cursorX.setText(Integer.toString(xPx));
				cursorY.setText(Integer.toString(yPx));
			}
			
			@Override
			public void showCursorColor(int r255, int g255, int b255)
			{
				cursorR.setText(Integer.toString(r255));
				cursorG.setText(Integer.toString(g255));
				cursorB.setText(Integer.toString(b255));
				
				Color color = new Color(r255 / 255.0, g255 / 255.0, b255 / 255.0, 1.0);
				colorRectangle.setFill(color);
			}
			
			@Override
			public void showZoom(float percentage)
			{
				percentageZoom.setText(Float.toString(percentage));
			}
		};
	}
	
	public DisplayInfoView getView()
	{
		return m_view;
	}
}
