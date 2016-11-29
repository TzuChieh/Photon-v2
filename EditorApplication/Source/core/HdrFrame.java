//	The MIT License (MIT)
//	
//	Copyright (c) 2016 Tzu-Chieh Chang (as known as D01phiN)
//	
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:
//	
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//	
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.

package core;

import photonCore.FrameData;

public class HdrFrame extends Frame
{
	private float[][][] m_pixelRgb;
	
	public HdrFrame(int widthPx, int heightPx)
	{
		super(widthPx, heightPx);
		
		m_pixelRgb = new float[widthPx][heightPx][3];
	}
	
	public HdrFrame(FrameData frameData)
	{
		super(frameData.getWidthPx(), frameData.getHeightPx());
		
		if(!frameData.isDataGood())
		{
			System.err.println("warning: at HdrFrame(), input FrameData is not good");
			return;
		}
		
		if(frameData.getNumPixelComponents() != 3)
		{
			System.err.println("warning: at HdrFrame(), input FrameData's number of pixel components != 3");
			return;
		}
		
		m_pixelRgb = new float[frameData.getWidthPx()][frameData.getHeightPx()][3];
		for(int x = 0; x < frameData.getWidthPx(); x++)
		{
			for(int y = 0; y < frameData.getHeightPx(); y++)
			{
				int baseIndex = (y * frameData.getWidthPx() + x) * 3;
				m_pixelRgb[x][y][R] = frameData.getPixelData()[baseIndex + 0];
				m_pixelRgb[x][y][G] = frameData.getPixelData()[baseIndex + 1];
				m_pixelRgb[x][y][B] = frameData.getPixelData()[baseIndex + 2];
			}
		}
	}
	
	@Override
	public float getPixelR(int x, int y)
	{
		return m_pixelRgb[x][y][R];
	}
	
	@Override
	public float getPixelG(int x, int y)
	{
		return m_pixelRgb[x][y][G];
	}
	
	@Override
	public float getPixelB(int x, int y)
	{
		return m_pixelRgb[x][y][B];
	}
	
	@Override
	public void setPixelRgb(int x, int y, float r, float g, float b)
	{
		m_pixelRgb[x][y][R] = r;
		m_pixelRgb[x][y][G] = g;
		m_pixelRgb[x][y][B] = b;
	}
	
	@Override
	public void set(Frame other)
	{
		for(int x = 0; x < getWidthPx(); x++)
		{
			for(int y = 0; y < getHeightPx(); y++)
			{
				m_pixelRgb[x][y][R] = other.getPixelR(x, y);
				m_pixelRgb[x][y][G] = other.getPixelG(x, y);
				m_pixelRgb[x][y][B] = other.getPixelB(x, y);
			}
		}
	}
}
