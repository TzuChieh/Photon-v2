package ui.taskStatus;

import javax.swing.JPanel;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import javax.swing.JLabel;
import javax.swing.SwingConstants;
import java.awt.Font;

@SuppressWarnings("serial")
public class TaskStatusPanel extends JPanel
{
	public static final Dimension PREFERRED_SIZE = new Dimension(350, 150);
	
	private String m_taskName;
	
	private PercentageProgressPanel m_progressPanel;
	private JLabel m_taskNameLabel;
	private JLabel m_sampleFrequencyLabel;
	
	public TaskStatusPanel(String taskName)
	{
		m_taskName = taskName;
		
		setForeground(Color.WHITE);
		setBackground(Color.BLACK);
		setLayout(new BorderLayout());
		
		m_progressPanel = new PercentageProgressPanel();
		add(m_progressPanel, BorderLayout.CENTER);
		
		m_taskNameLabel = new JLabel(taskName);
		m_taskNameLabel.setFont(new Font("Square721 BT", Font.BOLD, 18));
		m_taskNameLabel.setHorizontalAlignment(SwingConstants.CENTER);
		m_taskNameLabel.setBackground(Color.BLACK);
		m_taskNameLabel.setForeground(Color.WHITE);
		add(m_taskNameLabel, BorderLayout.NORTH);
		
		m_sampleFrequencyLabel = new JLabel("sample frequency: ");
		m_sampleFrequencyLabel.setHorizontalAlignment(SwingConstants.CENTER);
		m_sampleFrequencyLabel.setForeground(Color.WHITE);
		m_sampleFrequencyLabel.setFont(new Font("Square721 BT", Font.BOLD, 18));
		m_sampleFrequencyLabel.setBackground(Color.BLACK);
		add(m_sampleFrequencyLabel, BorderLayout.SOUTH);
	}
	
	@Override
	public Dimension getPreferredSize()
	{
		return PREFERRED_SIZE;
	}
	
	@Override
	public Dimension getMaximumSize()
	{
		return PREFERRED_SIZE;
	}
	
	@Override
	public Dimension getMinimumSize()
	{
		return PREFERRED_SIZE;
	}
}
