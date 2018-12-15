package jsdl;

import java.util.ArrayList;

public abstract class SDLCommand
{
	private ArrayList<SDLInput> m_inputs;
	
	protected SDLCommand()
	{
		m_inputs = new ArrayList<>();
	}
	
	public abstract String getPrefix();
	public abstract String getFullType();
	public abstract String generate();
	
	public void setInput(String name, SDLData data)
	{
		m_inputs.add(new SDLInput(name, data));
	}
	
	protected void generateInputs(StringBuilder out_fragments)
	{
		for(SDLInput input : m_inputs)
		{
			input.generateInput(out_fragments);
		}
	}
	
	private static class SDLInput
	{
		private String  m_name;
		private SDLData m_data;
		
		public SDLInput(String name, SDLData data)
		{
			m_name = name;
			m_data = data;
		}
		
		public void generateInput(StringBuilder out_fragments)
		{
			out_fragments.append('[');
			out_fragments.append(m_data.getType() + ' ');
			out_fragments.append(m_name + ' ');
			out_fragments.append(m_data.generateData());
			out_fragments.append(']');
		}
	}
}
