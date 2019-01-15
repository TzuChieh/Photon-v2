package appGui.util;

import java.io.File;
import java.io.IOException;

import javafx.stage.DirectoryChooser;
import javafx.stage.FileChooser;
import javafx.stage.Window;
import util.FSUtil;

public final class FSBrowser
{
	private Window m_ownerWindow;
	private String m_browserTitle;
	private File   m_startingDirectory;
	private File   m_selectedTarget;
	
	public FSBrowser(Window ownerWindow)
	{
		m_ownerWindow       = ownerWindow;
		m_browserTitle      = "File System Browser";
		m_startingDirectory = null;
		m_selectedTarget    = null;
	}
	
	public void startOpeningFile()
	{
		FileChooser chooser = new FileChooser();
		chooser.setTitle(m_browserTitle);
		chooser.setInitialDirectory(m_startingDirectory);
			    
		m_selectedTarget = chooser.showOpenDialog(m_ownerWindow);
	}
	
	public void startSavingFile()
	{
		FileChooser chooser = new FileChooser();
		chooser.setTitle(m_browserTitle);
		chooser.setInitialDirectory(m_startingDirectory);
			    
		m_selectedTarget = chooser.showSaveDialog(m_ownerWindow);
	}
	
	public void startOpeningDirectory()
	{
		DirectoryChooser chooser = new DirectoryChooser();
		chooser.setTitle(m_browserTitle);
		chooser.setInitialDirectory(m_startingDirectory);
			    
		m_selectedTarget = chooser.showDialog(m_ownerWindow);
	}
	
	public String getSelectedFileAbsPath()
	{
		String absPath = "";
		
		if(m_selectedTarget == null)
		{
			return absPath;
		}
		
		try
		{
			if(m_selectedTarget.isFile())
			{
				absPath = m_selectedTarget.getCanonicalPath();
			}
		}
		catch(IOException e)
		{
			e.printStackTrace();
		}
		
		return absPath;
	}
	
	public String getSelectedDirectoryAbsPath()
	{
		String absPath = "";
		
		if(m_selectedTarget == null)
		{
			return absPath;
		}
		
		try
		{
			if(m_selectedTarget.isDirectory())
			{
				absPath = m_selectedTarget.getCanonicalPath();
				absPath = FSUtil.toSeparatorEnded(absPath);
			}
		}
		catch(IOException e)
		{
			e.printStackTrace();
		}
		
		return absPath;
	}
	
	public void setBrowserTitle(String title)
	{
		m_browserTitle = title;
	}
	
	public void setStartingAbsDirectory(String directoryString)
	{
		File directory = new File(directoryString);
		if(directory.isDirectory())
		{
			m_startingDirectory = directory;
		}
	}
}
