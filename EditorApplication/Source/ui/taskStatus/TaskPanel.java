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

import ui.event.Event;
import ui.event.EventListener;
import ui.model.TaskStatusModel;

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
	
	public void registerTaskStatusModel(TaskStatusModel taskStatusModel)
	{
		TaskStatusPanel taskStatusPanel = new TaskStatusPanel();
		updateTaskStatusPanel(taskStatusPanel, taskStatusModel);
		addTaskStatusPanel(taskStatusPanel);
		
		taskStatusModel.addEventListener(new EventListener()
		{
			@Override
			public void eventTriggered(Event event)
			{
				updateTaskStatusPanel(taskStatusPanel, taskStatusModel);
			}
		});
	}
	
	private void addTaskStatusPanel(TaskStatusPanel taskStatusPanel)
	{
		m_scrolledPanel.add(taskStatusPanel);
		m_scrolledPanel.add(Box.createRigidArea(new Dimension(0, 5)));
	}
	
	private static void updateTaskStatusPanel(TaskStatusPanel taskStatusPanel, TaskStatusModel taskStatusModel)
	{
		taskStatusPanel.setTaskName(taskStatusModel.getTaskName());
		taskStatusPanel.setSampleFrequency(taskStatusModel.getSampleFrequency());
		taskStatusPanel.setPercentageProgress(taskStatusModel.getPercentageProgress());
	}
}
