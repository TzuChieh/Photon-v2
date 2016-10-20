package core;
import java.awt.BorderLayout;

import javax.swing.JFrame;
import javax.swing.SwingUtilities;

import ui.Display;
import ui.Window;

public class Main
{
	public static void main(String[] args)
	{
		System.out.println("Hello Editor!");
		
		
		SwingUtilities.invokeLater(new Runnable()
		{
			@Override
			public void run()
			{
				new Window(1280, 720);
			}
		});// end SwingUtilities.invokeLater()
		
		
		
		
		Ph.printTestMessage();
		
		System.out.println("Hello Editor!!!");
	}
}
