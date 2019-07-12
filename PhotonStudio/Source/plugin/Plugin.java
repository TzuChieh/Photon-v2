package plugin;

public abstract class Plugin
{
	protected Plugin()
	{}
	
	public abstract boolean init();
	public abstract void run();
	public abstract void stop();
	public abstract boolean exit();
	public abstract String getName();
}
