package ui.taskStatus;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.GridLayout;

import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.border.EtchedBorder;

@SuppressWarnings("serial")
public class TaskPanel extends JPanel
{
	private JPanel m_scrolledPanel;
	private JScrollPane m_taskStatusScrollPane;
	
	public TaskPanel()
	{
		setLayout(new BorderLayout());
		setBorder(new EtchedBorder(EtchedBorder.LOWERED, null, null));
		
		m_scrolledPanel = new JPanel();
		m_scrolledPanel.setLayout(new BoxLayout(m_scrolledPanel, BoxLayout.Y_AXIS));
		
		m_taskStatusScrollPane = new JScrollPane(m_scrolledPanel);
		m_taskStatusScrollPane.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
		add(m_taskStatusScrollPane, BorderLayout.CENTER);
	}
	
	public void addTaskStatusPanel(TaskStatusPanel taskStatusPanel)
	{
		m_scrolledPanel.add(taskStatusPanel);
		m_scrolledPanel.add(Box.createRigidArea(new Dimension(0, 5)));
	}
}
