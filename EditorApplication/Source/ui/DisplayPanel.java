package ui;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.JScrollPane;

import core.HdrFrame;
import javax.swing.border.EtchedBorder;
import java.awt.Color;

@SuppressWarnings("serial")
public class DisplayPanel extends JPanel
{
	//private JScrollPane m_frameScrollPane;
	private FramePanel m_framePanel;
	private ImageIcon m_displayImageIcon;
	
	private JPanel m_controlPanel;
	private JButton m_requestIntermeidateResultButton;
	
	public DisplayPanel(int xPx, int yPx, int widthPx, int heightPx)
	{
		super();
		setBorder(new EtchedBorder(EtchedBorder.LOWERED, null, null));
		setLayout(new BorderLayout());
		setBounds(xPx, yPx, widthPx, heightPx);
		setPreferredSize(new Dimension(widthPx, heightPx));
		
		m_framePanel = new FramePanel();
		m_framePanel.setBackground(new Color(0, 0, 50));
		m_framePanel.setForeground(Color.YELLOW);
		//m_frameScrollPane = new JScrollPane(m_framePanel);
		add(m_framePanel, BorderLayout.CENTER);
		
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
