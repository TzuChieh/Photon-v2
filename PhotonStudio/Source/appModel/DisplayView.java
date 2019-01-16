package appModel;

public interface DisplayView
{
	default void showFrameResolution(int widthPx, int heightPx)
	{}
	
	default void showDisplayResolution(int widthPx, int heightPx)
	{}
	
	default void showCursorCoord(int xPx, int yPx)
	{}
	
	default void showCursorColor(int r255, int g255, int b255)
	{}
	
	default void showZoom(float percentage)
	{}
}
