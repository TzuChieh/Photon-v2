package ui;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.JScrollPane;

import core.HdrFrame;
import javax.swing.border.EtchedBorder;

@SuppressWarnings("serial")
public class DisplayPanel extends JPanel
{
	private JScrollPane m_frameScrollPane;
	private FramePanel m_framePanel;
	
	private JPanel m_controlPanel;
	private JButton m_requestIntermeidateResultButton;
	
	public DisplayPanel(int xPx, int yPx, int widthPx, int heightPx)
	{
		super();
		setBorder(new EtchedBorder(EtchedBorder.LOWERED, null, null));
		setLayout(new BorderLayout());
		setBounds(xPx, yPx, widthPx, heightPx);
		setPreferredSize(new Dimension(widthPx, heightPx));
		
		m_framePanel = new FramePanel(900, 900);
		m_frameScrollPane = new JScrollPane(m_framePanel);
		add(m_frameScrollPane, BorderLayout.CENTER);
		
		// control panel
		
		m_controlPanel = new JPanel();
		m_controlPanel.setLayout(new FlowLayout());
		add(m_controlPanel, BorderLayout.SOUTH);
		
		m_requestIntermeidateResultButton = new JButton("Request Intermediate Result");
		m_requestIntermeidateResultButton.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent arg0)
			{
				System.out.println("clicked");
			}
		});
		m_controlPanel.add(m_requestIntermeidateResultButton);
	}
	
	public void render(HdrFrame frame)
	{
		m_framePanel.render(frame);
	}
}
