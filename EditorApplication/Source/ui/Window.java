package ui;

import javax.swing.JFrame;
import javax.swing.border.EtchedBorder;

import core.HdrFrame;
import ui.logger.LoggerTextPane;
import ui.taskStatus.TaskPanel;
import ui.taskStatus.TaskStatusPanel;

import java.awt.Color;
import java.awt.Font;

@SuppressWarnings("serial")
public class Window extends JFrame
{
	public static final int WIDTH_PX = 1280;
	public static final int HEIGHT_PX = 700;
	
	private DisplayPanel m_displayPanel;
	private LoggerTextPane m_loggerTextPane;
	private TaskPanel m_taskPanel;
	
	public Window(int widthPx, int heightPx)
	{
		super("Photon-v2 Editor");
		
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setSize(WIDTH_PX, HEIGHT_PX);
		setLocationRelativeTo(null);
		setResizable(false);
		setVisible(true);
		getContentPane().setLayout(null);
		
		m_displayPanel = new DisplayPanel(374, 0, 900, 500);
		//m_displayPanel.setBounds(551, 0, 500, 421);
		getContentPane().add(m_displayPanel);
		
		
		//m_jframe.add(m_display, BorderLayout.CENTER);
		
		
		m_loggerTextPane = new LoggerTextPane();
		m_loggerTextPane.setEnabled(true);
		m_loggerTextPane.setEditable(false);
		m_loggerTextPane.setForeground(Color.WHITE);
		m_loggerTextPane.setFont(new Font("Consolas", Font.PLAIN, 12));
		m_loggerTextPane.setBackground(Color.BLACK);
		m_loggerTextPane.setBounds(0, 448, 370, 224);
		m_loggerTextPane.setBorder(new EtchedBorder(EtchedBorder.LOWERED, null, null));
		m_loggerTextPane.directSystemOutErrToThis();
		getContentPane().add(m_loggerTextPane);
		
		m_taskPanel = new TaskPanel();
		m_taskPanel.setBounds(0, 0, 370, 443);
//		m_taskPanel.addTaskStatusPanel(new TaskStatusPanel("test task name"));
//		m_taskPanel.addTaskStatusPanel(new TaskStatusPanel("test task name"));
		getContentPane().add(m_taskPanel);
		
		
		revalidate();
	}
	
	public DisplayPanel getDisplayPanel()
	{
		return m_displayPanel;
	}
	
	public TaskPanel getTaskPanel()
	{
		return m_taskPanel;
	}
}
