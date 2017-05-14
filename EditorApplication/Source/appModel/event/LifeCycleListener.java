package appModel.event;

public interface LifeCycleListener
{
	public abstract void onCreate(LifeCycleEvent event);
	public abstract void onDecompose(LifeCycleEvent event);
}
