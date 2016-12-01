package ui.event;

public interface EventDispatcher
{
	public abstract void dispatchEvent(Event event);
	public abstract void addEventListener(EventListener listener);
}
