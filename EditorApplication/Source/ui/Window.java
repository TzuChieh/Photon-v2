package ui;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.GraphicsEnvironment;
import java.awt.Rectangle;
import java.awt.Toolkit;

import javax.swing.JFrame;
import javax.swing.JScrollPane;
import javax.swing.SwingUtilities;

import core.HdrFrame;
import photonApi.FloatArrayRef;
import photonApi.IntRef;
import photonApi.Ph;

public class Window
{
	public static final int WIDTH_PX = 1280;
	public static final int HEIGHT_PX = 700;
	
	private JFrame m_jframe;
	private DisplayPanel m_displayPanel;
	
	public Window(int widthPx, int heightPx)
	{
		m_jframe = new JFrame("Photon-v2 Editor");
		m_jframe.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		m_jframe.setSize(WIDTH_PX, HEIGHT_PX);
		m_jframe.setLocationRelativeTo(null);
		//m_jframe.setLayout(new AbsoluteLayout());
		m_jframe.setResizable(false);
		m_jframe.setVisible(true);
		m_jframe.getContentPane().setLayout(null);
		
		m_displayPanel = new DisplayPanel(374, 0, 900, 500);
		//m_displayPanel.setBounds(551, 0, 500, 421);
		m_jframe.getContentPane().add(m_displayPanel);
		
		
		//m_jframe.add(m_display, BorderLayout.CENTER);
		
		
		
		m_jframe.revalidate();
	}
	
	public void render(HdrFrame frame)
	{
		m_displayPanel.render(frame);
	}
}
