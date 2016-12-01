package ui.taskStatus;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics;
import java.text.DecimalFormat;
import java.util.Arrays;

import javax.swing.JPanel;
import javax.swing.JLabel;
import javax.swing.SwingConstants;

@SuppressWarnings("serial")
public class PercentageProgressPanel extends JPanel
{	
	private static final DecimalFormat PROGRESS_FORMAT = new DecimalFormat("0.00");
	
	private JLabel m_percentageLabel;
	
	public PercentageProgressPanel()
	{
		setForeground(Color.WHITE);
		setBackground(Color.BLACK);
		setLayout(new BorderLayout());
		
		m_percentageLabel = new JLabel();
		m_percentageLabel.setHorizontalAlignment(SwingConstants.CENTER);
		m_percentageLabel.setForeground(new Color(0, 191, 255));
		m_percentageLabel.setFont(new Font("Square721 BT", Font.BOLD, 40));
		add(m_percentageLabel, BorderLayout.CENTER);
		
		setPercentage(0.0f);
	}
	
	public void setPercentage(float percentage)
	{
		m_percentageLabel.setText(PROGRESS_FORMAT.format(percentage) + " %");
	}
	
//	private static final int LED_STATE_OFF = 0;
//	private static final int LED_STATE_ON  = 1;
//	
//	private int m_ledSizePx;
//	private int m_ledIntervalPx;
//	private int m_numberIntervalPx;
//	
//	private int m_integerPart;
//	private int m_decimalPart;
//	
//	public PercentageProgressPanel(int ledSizePx, int ledInvervalPx, int numberIntervalPx)
//	{
//		super();
//		
//		m_ledSizePx = ledSizePx;
//		m_ledIntervalPx = ledInvervalPx;
//		m_numberIntervalPx = numberIntervalPx;
//		
//		m_integerPart = 34;
//		m_decimalPart = 89;
//		
//		setForeground(Color.WHITE);
//		setBackground(Color.BLACK);
//	}
//	
//	protected void paintComponent(Graphics g)
//	{
//	    super.paintComponent(g);
//	    
//	    int x = 0;
//	    int y = 0;
//	    int numberWidthPx  = m_ledSizePx * 3 + m_ledIntervalPx * 2;
//	    int numberHeightPx = m_ledSizePx * 5 + m_ledIntervalPx * 4;
//	    Color ledColor = new Color(0, 191, 255);
//	    
//	    // draw integer part
//	    
//	    drawDigit(x, y, getDigit(m_integerPart, 1), g, ledColor);
//	    
//	    x += (numberWidthPx + m_numberIntervalPx);
//	    drawDigit(x, y, getDigit(m_integerPart, 0), g, ledColor);
//	    
//	    // draw decimal point
//	    
//	    x += (numberWidthPx + m_numberIntervalPx);
//	    drawLed(x, y + numberHeightPx - m_ledSizePx, g, ledColor, LED_STATE_ON);
//	    
//	    // draw decimal part
//	    
//	    x += (m_ledSizePx + m_numberIntervalPx);
//	    drawDigit(x, y, getDigit(m_decimalPart, 1), g, ledColor);
//	    
//	    x += (numberWidthPx + m_numberIntervalPx);
//	    drawDigit(x, y, getDigit(m_decimalPart, 0), g, ledColor);
//	    
//	    g.setFont(new Font("Square721 BT", Font.BOLD, 40));
//	    g.drawString("55.43%", 100, 100);
//	    
//	}
//	
//	public void setIntegerPart(int integerPart)
//	{
//		m_integerPart = integerPart;
//	}
//	
//	public void setDecimalPart(int decimalPart)
//	{
//		m_decimalPart = decimalPart;
//	}
//	
//	private void drawDigit(int x, int y, int digit, Graphics g, Color ledColor)
//	{
//		int[] s = new int[3 * 5];
//		Arrays.fill(s, LED_STATE_OFF);
//		
//		switch(digit)
//		{
//		case 0:
//			s[0]  = LED_STATE_ON; s[1]  = LED_STATE_ON;  s[2]  = LED_STATE_ON;
//			s[3]  = LED_STATE_ON; s[4]  = LED_STATE_OFF; s[5]  = LED_STATE_ON;
//			s[6]  = LED_STATE_ON; s[7]  = LED_STATE_OFF; s[8]  = LED_STATE_ON;
//			s[9]  = LED_STATE_ON; s[10] = LED_STATE_OFF; s[11] = LED_STATE_ON;
//			s[12] = LED_STATE_ON; s[13] = LED_STATE_ON;  s[14] = LED_STATE_ON;
//			break;
//		
//		case 1:
//			s[0]  = LED_STATE_OFF; s[1]  = LED_STATE_OFF; s[2]  = LED_STATE_ON;
//			s[3]  = LED_STATE_OFF; s[4]  = LED_STATE_OFF; s[5]  = LED_STATE_ON;
//			s[6]  = LED_STATE_OFF; s[7]  = LED_STATE_OFF; s[8]  = LED_STATE_ON;
//			s[9]  = LED_STATE_OFF; s[10] = LED_STATE_OFF; s[11] = LED_STATE_ON;
//			s[12] = LED_STATE_OFF; s[13] = LED_STATE_OFF; s[14] = LED_STATE_ON;
//			break;
//			
//		case 2:
//			s[0]  = LED_STATE_ON;  s[1]  = LED_STATE_ON;  s[2]  = LED_STATE_ON;
//			s[3]  = LED_STATE_OFF; s[4]  = LED_STATE_OFF; s[5]  = LED_STATE_ON;
//			s[6]  = LED_STATE_ON;  s[7]  = LED_STATE_ON;  s[8]  = LED_STATE_ON;
//			s[9]  = LED_STATE_ON;  s[10] = LED_STATE_OFF; s[11] = LED_STATE_OFF;
//			s[12] = LED_STATE_ON;  s[13] = LED_STATE_ON;  s[14] = LED_STATE_ON;
//			break;
//			
//		case 3:
//			s[0]  = LED_STATE_ON;  s[1]  = LED_STATE_ON;  s[2]  = LED_STATE_ON;
//			s[3]  = LED_STATE_OFF; s[4]  = LED_STATE_OFF; s[5]  = LED_STATE_ON;
//			s[6]  = LED_STATE_ON;  s[7]  = LED_STATE_ON;  s[8]  = LED_STATE_ON;
//			s[9]  = LED_STATE_OFF; s[10] = LED_STATE_OFF; s[11] = LED_STATE_ON;
//			s[12] = LED_STATE_ON;  s[13] = LED_STATE_ON;  s[14] = LED_STATE_ON;
//			break;
//			
//		case 4:
//			s[0]  = LED_STATE_ON;  s[1]  = LED_STATE_OFF; s[2]  = LED_STATE_ON;
//			s[3]  = LED_STATE_ON;  s[4]  = LED_STATE_OFF; s[5]  = LED_STATE_ON;
//			s[6]  = LED_STATE_ON;  s[7]  = LED_STATE_ON;  s[8]  = LED_STATE_ON;
//			s[9]  = LED_STATE_OFF; s[10] = LED_STATE_OFF; s[11] = LED_STATE_ON;
//			s[12] = LED_STATE_OFF; s[13] = LED_STATE_OFF; s[14] = LED_STATE_ON;
//			break;
//			
//		case 5:
//			s[0]  = LED_STATE_ON;  s[1]  = LED_STATE_ON;  s[2]  = LED_STATE_ON;
//			s[3]  = LED_STATE_ON;  s[4]  = LED_STATE_OFF; s[5]  = LED_STATE_OFF;
//			s[6]  = LED_STATE_ON;  s[7]  = LED_STATE_ON;  s[8]  = LED_STATE_ON;
//			s[9]  = LED_STATE_OFF; s[10] = LED_STATE_OFF; s[11] = LED_STATE_ON;
//			s[12] = LED_STATE_ON;  s[13] = LED_STATE_ON;  s[14] = LED_STATE_ON;
//			break;
//			
//		case 6:
//			s[0]  = LED_STATE_ON; s[1]  = LED_STATE_ON;  s[2]  = LED_STATE_ON;
//			s[3]  = LED_STATE_ON; s[4]  = LED_STATE_OFF; s[5]  = LED_STATE_OFF;
//			s[6]  = LED_STATE_ON; s[7]  = LED_STATE_ON;  s[8]  = LED_STATE_ON;
//			s[9]  = LED_STATE_ON; s[10] = LED_STATE_OFF; s[11] = LED_STATE_ON;
//			s[12] = LED_STATE_ON; s[13] = LED_STATE_ON;  s[14] = LED_STATE_ON;
//			break;
//			
//		case 7:
//			s[0]  = LED_STATE_ON;  s[1]  = LED_STATE_ON;  s[2]  = LED_STATE_ON;
//			s[3]  = LED_STATE_OFF; s[4]  = LED_STATE_OFF; s[5]  = LED_STATE_ON;
//			s[6]  = LED_STATE_OFF; s[7]  = LED_STATE_OFF; s[8]  = LED_STATE_ON;
//			s[9]  = LED_STATE_OFF; s[10] = LED_STATE_OFF; s[11] = LED_STATE_ON;
//			s[12] = LED_STATE_OFF; s[13] = LED_STATE_OFF; s[14] = LED_STATE_ON;
//			break;
//			
//		case 8:
//			s[0]  = LED_STATE_ON; s[1]  = LED_STATE_ON;  s[2]  = LED_STATE_ON;
//			s[3]  = LED_STATE_ON; s[4]  = LED_STATE_OFF; s[5]  = LED_STATE_ON;
//			s[6]  = LED_STATE_ON; s[7]  = LED_STATE_ON;  s[8]  = LED_STATE_ON;
//			s[9]  = LED_STATE_ON; s[10] = LED_STATE_OFF; s[11] = LED_STATE_ON;
//			s[12] = LED_STATE_ON; s[13] = LED_STATE_ON;  s[14] = LED_STATE_ON;
//			break;
//			
//		case 9:
//			s[0]  = LED_STATE_ON;  s[1]  = LED_STATE_ON;  s[2]  = LED_STATE_ON;
//			s[3]  = LED_STATE_ON;  s[4]  = LED_STATE_OFF; s[5]  = LED_STATE_ON;
//			s[6]  = LED_STATE_ON;  s[7]  = LED_STATE_ON;  s[8]  = LED_STATE_ON;
//			s[9]  = LED_STATE_OFF; s[10] = LED_STATE_OFF; s[11] = LED_STATE_ON;
//			s[12] = LED_STATE_ON;  s[13] = LED_STATE_ON;  s[14] = LED_STATE_ON;
//			break;
//			
//		default:
//			System.err.println("warning: bad digit number");
//			break;
//		}
//		
//		drawDigitLeds(x, y, g, ledColor, s);
//	}
//	
//	private void drawDigitLeds(int x, int y, Graphics g, Color ledColor, int[] ledStates)
//	{
//		for(int iy= 0; iy < 5; iy++)
//		{
//			for(int ix = 0; ix < 3; ix++)
//			{
//				int ledState = ledStates[iy * 3 + ix];
//				drawLed(x + ix * (m_ledSizePx + m_ledIntervalPx), y + iy * (m_ledSizePx + m_ledIntervalPx), g, ledColor, ledState);
//			}
//		}
//	}
//	
//	private void drawLed(int x, int y, Graphics g, Color ledColor, int ledState)
//	{
//		if(ledState == LED_STATE_OFF)
//		{
//			// do nothing
//		}
//		else if(ledState == LED_STATE_ON)
//		{
//			g.setColor(ledColor);
//		    g.fillRect(x, y, m_ledSizePx, m_ledSizePx);
//		}
//		else
//		{
//			System.err.println("bad led state");
//		}
//	}
//	
//	private static int getDigit(int number, int i)
//	{
//		while(i > 0)
//		{
//			number /= 10;
//			i--;
//		}
//		
//		return number % 10;
//	}
}
