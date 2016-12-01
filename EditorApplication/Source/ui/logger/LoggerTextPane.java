package ui.logger;

import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintStream;

import javax.swing.JTextPane;
import javax.swing.SwingUtilities;
import javax.swing.text.BadLocationException;
import javax.swing.text.Document;

@SuppressWarnings("serial")
public class LoggerTextPane extends JTextPane
{
	public LoggerTextPane()
	{
		
	}
	
	public void directSystemOutErrToThis()
	{
		OutputStream myOut = new OutputStream()
		{
		    @Override
		    public void write(final int b) throws IOException
		    {
		    	updateTextPane(String.valueOf((char)b));
		    }
		 
		    @Override
		    public void write(byte[] b, int off, int len) throws IOException
		    {
		    	updateTextPane(new String(b, off, len));
		    }
		 
		    @Override
		    public void write(byte[] b) throws IOException
		    {
		    	write(b, 0, b.length);
		    }
		};
		 
		System.setOut(new PrintStream(myOut, true));
		System.setErr(new PrintStream(myOut, true));
	}
	
	private void updateTextPane(final String text)
	{
		final JTextPane me = this;
		SwingUtilities.invokeLater(new Runnable()
		{
			public void run()
			{
				Document doc = me.getDocument();
				try
				{
					doc.insertString(doc.getLength(), text, null);
				}
				catch(BadLocationException e)
				{
					throw new RuntimeException(e);
				}
				me.setCaretPosition(doc.getLength() - 1);
			}
		});
	}
}
