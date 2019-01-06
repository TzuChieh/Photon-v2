package jsdl;

public interface SDLData
{
	public abstract String getType();
	public abstract void generateData(StringBuilder out_sdlBuffer);
}
