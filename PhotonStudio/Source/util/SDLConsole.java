package util;

import java.io.FileNotFoundException;
import java.io.PrintWriter;

import jsdl.SDLCommand;

public class SDLConsole
{
	private String        m_name;
	private StringBuilder m_sdlBuffer;
	private PrintWriter   m_sdlWriter;
	
	public SDLConsole(String name)
	{
		m_name      = name;
		m_sdlBuffer = new StringBuilder();
		m_sdlWriter = null;
	}
	
	public void start()
	{
		try
		{
			m_sdlWriter = new PrintWriter("./" + m_name + ".p2");
		}
		catch(FileNotFoundException e)
		{
			e.printStackTrace();
		}
	}
	
	public void exit()
	{
		if(m_sdlWriter != null)
		{
			flushSDLBuffer();
			m_sdlWriter.close();
		}
	}
	
	public void queue(SDLCommand command)
	{
		command.generate(m_sdlBuffer);
		
		if(m_sdlBuffer.length() > 8192)
		{
			flushSDLBuffer();
		}
	}
	
	private void flushSDLBuffer()
	{
		m_sdlWriter.append(m_sdlBuffer);
		m_sdlBuffer.setLength(0);
	}
}
