package minecraft.block;

import java.util.Arrays;

public class AllMatchedConditional implements StateConditional
{
	private StateAggregate m_targetStates;
	
	public AllMatchedConditional()
	{
		m_targetStates = new StateAggregate();
	}
	
	public void addTargetState(String name, String value)
	{
		m_targetStates.add(name, value);
	}
	
	/**
	 * Add multiple target states from comma separated key-value pairs. 
	 * @param states Comma separated key-value pairs in the form key1=value1,key2=value2,...
	 */
	public void addTargetStates(String states)
	{
		if(states.isEmpty())
		{
			return;
		}
		
		String[] keyValuePairs = states.split("[,]");
		for(int i = 0; i < keyValuePairs.length; ++i)
		{
			keyValuePairs[i] = keyValuePairs[i].trim();
		}
		Arrays.sort(keyValuePairs);

		for(String keyValuePair : keyValuePairs)
		{
			String[] keyValue = keyValuePair.split("[=]");
			addTargetState(keyValue[0].trim(), keyValue[1].trim());
		}
	}
	
	@Override
	public boolean evaluate(StateAggregate states)
	{
		return m_targetStates.isMatching(states);
	}
}
