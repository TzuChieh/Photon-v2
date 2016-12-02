package ui.display;

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
import java.awt.Font;

@SuppressWarnings("serial")
public class DisplayPanel extends JPanel
{
	private ImagePanel m_imagePanel;
	
	private JPanel m_controlPanel;
	private JButton m_requestIntermeidateResultButton;
	private JButton m_saveImageButton;
	
	public DisplayPanel(int xPx, int yPx, int widthPx, int heightPx)
	{
		super();
		setBorder(new EtchedBorder(EtchedBorder.LOWERED, null, null));
		setLayout(new BorderLayout());
		setBounds(xPx, yPx, widthPx, heightPx);
		setPreferredSize(new Dimension(widthPx, heightPx));
		
		m_imagePanel = new ImagePanel();
		m_imagePanel.setBackground(new Color(0, 0, 50));
		m_imagePanel.setForeground(Color.YELLOW);
		add(m_imagePanel, BorderLayout.CENTER);
		
		// control panel
		
		m_controlPanel = new JPanel();
		m_controlPanel.setLayout(new FlowLayout());
		add(m_controlPanel, BorderLayout.SOUTH);
		
		m_requestIntermeidateResultButton = new JButton("Request Intermediate Result");
		m_requestIntermeidateResultButton.setFont(new Font("Verdana", Font.BOLD, 12));
		m_requestIntermeidateResultButton.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent arg0)
			{
				System.err.println("function not implemented");
			}
		});
		m_controlPanel.add(m_requestIntermeidateResultButton);
		
		m_saveImageButton = new JButton("Save Image");
		m_saveImageButton.setFont(new Font("Verdana", Font.BOLD, 12));
		m_saveImageButton.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent arg0)
			{
				m_imagePanel.saveImage("./test");
			}
		});
		m_controlPanel.add(m_saveImageButton);
	}
	
	public void render(HdrFrame frame)
	{
		m_imagePanel.render(frame);
	}
}
