package ui;

import java.awt.BorderLayout;
import java.awt.Dimension;

import javax.swing.JPanel;
import javax.swing.JScrollPane;

import core.HdrFrame;

@SuppressWarnings("serial")
public class DisplayPanel extends JPanel
{
	private JScrollPane m_frameScrollPane;
	private FramePanel m_framePanel;
	
	public DisplayPanel(int xPx, int yPx, int widthPx, int heightPx)
	{
		super();
		
		this.setLayout(new BorderLayout());
		this.setBounds(xPx, yPx, widthPx, heightPx);
		this.setPreferredSize(new Dimension(widthPx, heightPx));
		
		
		m_framePanel = new FramePanel(900, 900);
		m_frameScrollPane = new JScrollPane(m_framePanel);
		this.add(m_frameScrollPane, BorderLayout.CENTER);
	}
	
	public void render(HdrFrame frame)
	{
		m_framePanel.render(frame);
	}
}
