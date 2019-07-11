package plugin.minecraft;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.logging.Formatter;
import java.util.logging.Handler;
import java.util.logging.Level;
import java.util.logging.LogRecord;
import java.util.logging.Logger;
import java.util.logging.StreamHandler;

import util.Time;

public class MCLogger
{
	private static final Logger LOG  = Logger.getLogger("Photon MC");
	private static final Logger WARN = Logger.getLogger("Photon MC: Warning");
	
	private static final DateFormat DATE_FORMAT = new SimpleDateFormat("dd MMM yyyy HH:mm:ss:SSS");
	
	static
	{
		LOG.setUseParentHandlers(false);
		for(Handler handler : LOG.getHandlers())
		{
			LOG.removeHandler(handler);
		}
		
		Formatter formatter = new Formatter()
		{
			@Override
			public String format(LogRecord record)
			{
				return "[" + DATE_FORMAT.format(record.getMillis()) + "] ["+ record.getLoggerName() + "] " + record.getMessage() + "\n";
			}
		};
		
		LOG.addHandler(new StreamHandler(System.out, formatter));
	}
	
	public static void log(Object data)
	{
		// FIXME: buffers too much
		//LOG.log(Level.INFO, data.toString());
		
		System.out.println("[" + DATE_FORMAT.format(Time.getTimeMs()) + "] [Photon MC] " + data.toString());
	}
	
	public static void warn(Object data)
	{
		WARN.log(Level.WARNING, data.toString());
	}
}
