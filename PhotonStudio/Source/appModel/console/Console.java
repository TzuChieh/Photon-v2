package appModel.console;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;

public final class Console
{
	private LinkedList<String>    m_cachedMessages;
	private List<MessageListener> m_listeners;
	
	public Console(int maxMessages)
	{
		m_cachedMessages = new LinkedList<>();
		m_listeners      = new ArrayList<>();
		
		for(int i = 0; i < maxMessages; i++)
		{
			m_cachedMessages.addLast("");
		}
	}
	
	public synchronized void writeMessage(String message)
	{
		m_cachedMessages.removeFirst();
		m_cachedMessages.addLast(message);
		
		for(MessageListener listener : m_listeners)
		{
			listener.onMessageWritten(message);
		}
	}
	
	public synchronized void getCachedMessages(StringBuilder builder)
	{
		for(String message : m_cachedMessages)
		{
			builder.append(message).append('\n');
		}
	}
	
	public synchronized void addListener(MessageListener targetListener)
	{
		m_listeners.add(targetListener);
	}
	
	public synchronized void removeListener(MessageListener targetListener)
	{
		m_listeners.remove(targetListener);
	}
}
