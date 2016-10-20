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

public class Color 
{
	public static byte getRedByte(int ARGB)
	{
		return (byte)((ARGB >> 16) & 0xFF);
	}
	
	public static byte getGreenByte(int ARGB)
	{
		return (byte)((ARGB >> 8) & 0xFF);
	}
	
	public static byte getBlueByte(int ARGB)
	{
		return (byte)((ARGB) & 0xFF);
	}
	
	public static byte getAlphaByte(int ARGB)
	{
		return (byte)((ARGB >> 24) & 0xFF);
	}
	
	public static float getRedNormalized(int ARGB)
	{
		return (float)(getRedByte(ARGB) & 0xFF) / 255.0f;
	}
	
	public static float getGreenNormalized(int ARGB)
	{
		return (float)(getGreenByte(ARGB) & 0xFF) / 255.0f;
	}
	
	public static float getBlueNormalized(int ARGB)
	{
		return (float)(getBlueByte(ARGB) & 0xFF) / 255.0f;
	}
	
	public static float getAlphaNormalized(int ARGB)
	{
		return (float)(getAlphaByte(ARGB) & 0xFF) / 255.0f;
	}
	
	public static int toRGBInt(int red255, int green255, int blue255)
	{
		return (red255 << 16) | (green255 << 8) | blue255;
	}
	
	public static int toARGBInt(int alpha255, int red255, int green255, int blue255)
	{
		return (alpha255 << 24) | (red255 << 16) | (green255 << 8) | blue255;
	}
	
	public static int toRGBInt(float r, float g, float b)
	{
		int r255 = (int)(r * 255.0f + 0.5f);
		int g255 = (int)(g * 255.0f + 0.5f);
		int b255 = (int)(b * 255.0f + 0.5f);
		
		return (r255 << 16) | (g255 << 8) | b255;
	}
	
	public static int toARGBInt(float a, float r, float g, float b)
	{
		int a255 = (int)(a * 255.0f + 0.5f);
		int r255 = (int)(r * 255.0f + 0.5f);
		int g255 = (int)(g * 255.0f + 0.5f);
		int b255 = (int)(b * 255.0f + 0.5f);
		
		return (a255 << 24) | (r255 << 16) | (g255 << 8) | b255;
	}
}
