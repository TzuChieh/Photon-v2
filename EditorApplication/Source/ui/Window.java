package ui;

import javax.swing.JFrame;
import javax.swing.border.EtchedBorder;

import core.HdrFrame;
import ui.display.DisplayPanel;
import ui.logger.LoggerTextPane;
import ui.task.TaskDetail;
import ui.task.TaskPanel;
import ui.task.TaskStatusPanel;

import java.awt.Color;
import java.awt.Font;
import java.awt.GraphicsEnvironment;
import java.awt.Rectangle;
import java.awt.FlowLayout;
import javax.swing.JSplitPane;
import java.awt.BorderLayout;

@SuppressWarnings("serial")
public class Window extends JFrame
{
	private DisplayPanel m_displayPanel;
	private LoggerTextPane m_loggerTextPane;
	private TaskPanel m_taskPanel;
	private JSplitPane m_taskDetailDisplaySplitPane;
	
	private TaskDetail m_taskDetail;
	
	public Window()
	{
		super();
		
		Rectangle bounds = GraphicsEnvironment.getLocalGraphicsEnvironment().getMaximumWindowBounds();
		setBounds(bounds);
		setTitle("Photon-v2 Editor");
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setLocationRelativeTo(null);
		setResizable(true);
		setVisible(true);
		getContentPane().setLayout(new BorderLayout(0, 0));
		
		
		
		
		
		
//		m_loggerTextPane = new LoggerTextPane();
//		m_loggerTextPane.setEnabled(true);
//		m_loggerTextPane.setEditable(false);
//		m_loggerTextPane.setForeground(Color.WHITE);
//		m_loggerTextPane.setFont(new Font("Consolas", Font.PLAIN, 12));
//		m_loggerTextPane.setBackground(Color.BLACK);
//		m_loggerTextPane.setBorder(new EtchedBorder(EtchedBorder.LOWERED, null, null));
		//m_loggerTextPane.directSystemOutErrToThis();
//		getContentPane().add(m_loggerTextPane);
		
		m_taskPanel = new TaskPanel();
		getContentPane().add(m_taskPanel, BorderLayout.NORTH);
		
		
		
		
		
		m_taskDetailDisplaySplitPane = new JSplitPane();
		getContentPane().add(m_taskDetailDisplaySplitPane, BorderLayout.CENTER);
		
		m_taskDetail = new TaskDetail();
		m_displayPanel = new DisplayPanel(374, 0, 900, 500);
		m_taskDetailDisplaySplitPane.setLeftComponent(m_taskDetail.getPanel());
		m_taskDetailDisplaySplitPane.setRightComponent(m_displayPanel);
		
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
