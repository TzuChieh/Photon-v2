package ui.taskStatus;

import javax.swing.JPanel;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import javax.swing.JLabel;
import javax.swing.SwingConstants;
import java.awt.Font;
import java.text.DecimalFormat;

@SuppressWarnings("serial")
public class TaskStatusPanel extends JPanel
{
	public static final Dimension PREFERRED_SIZE = new Dimension(350, 100);
	
	private static final DecimalFormat PROGRESS_FORMAT  = new DecimalFormat("0.00");
	//private static final DecimalFormat FREQUENCY_FORMAT = new DecimalFormat("0.00");
	
	private JLabel m_progressPanel;
	private JLabel m_taskNameLabel;
	private JLabel m_sampleFrequencyLabel;
	
	public TaskStatusPanel()
	{
		setForeground(Color.WHITE);
		setBackground(Color.BLACK);
		setLayout(new BorderLayout());
		
		m_taskNameLabel = new JLabel();
		m_taskNameLabel.setFont(new Font("Square721 BT", Font.BOLD, 18));
		m_taskNameLabel.setHorizontalAlignment(SwingConstants.CENTER);
		m_taskNameLabel.setBackground(Color.BLACK);
		m_taskNameLabel.setForeground(Color.WHITE);
		add(m_taskNameLabel, BorderLayout.NORTH);
		
		m_progressPanel = new JLabel();
		m_progressPanel.setForeground(new Color(0, 191, 255));
		m_progressPanel.setBackground(Color.BLACK);
		m_progressPanel.setHorizontalAlignment(SwingConstants.CENTER);
		m_progressPanel.setFont(new Font("Square721 BT", Font.BOLD, 40));
		add(m_progressPanel, BorderLayout.CENTER);
		
		m_sampleFrequencyLabel = new JLabel("sample frequency: ");
		m_sampleFrequencyLabel.setHorizontalAlignment(SwingConstants.CENTER);
		m_sampleFrequencyLabel.setForeground(Color.WHITE);
		m_sampleFrequencyLabel.setFont(new Font("Square721 BT", Font.BOLD, 18));
		m_sampleFrequencyLabel.setBackground(Color.BLACK);
		add(m_sampleFrequencyLabel, BorderLayout.SOUTH);
	}
	
	public void setTaskName(String taskName)
	{
		m_taskNameLabel.setText(taskName);
	}
	
	public void setSampleFrequency(float sampleFrequency)
	{
		m_sampleFrequencyLabel.setText((int)(sampleFrequency / 1000.0f) + "k samples / sec");
	}
	
	public void setPercentageProgress(float percentageProgress)
	{
		m_progressPanel.setText(PROGRESS_FORMAT.format(percentageProgress) + " %");
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
