package appView;

public interface RenderStatusView
{
	default void showProgress(float normalizedProgress)
	{}
	
	default void showTimeRemaining(long ms)
	{}
	
	default void showTimeSpent(long ms)
	{}
	
	default void showStates(String[] names, String[] values)
	{}
}
