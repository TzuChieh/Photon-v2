package appModel;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import appModel.event.SettingEvent;
import appModel.event.SettingListener;

public abstract class SettingGroup
{
	public static final int INVALID_ID = -1;
	
	private Map<Integer, String> m_settings;
	private List<SettingListener> m_settingListeners;
	
	protected SettingGroup()
	{
		m_settings = new HashMap<>();
		m_settingListeners = new ArrayList<>();
	}
	
	public String get(int settingId)
	{
		return m_settings.get(settingId);
	}
	
	public void set(int settingId, String settingValue)
	{
		m_settings.put(settingId, settingValue);
		
		for(SettingListener listener : m_settingListeners)
		{
			SettingEvent event = new SettingEvent();
			event.source       = this;
			event.settingId    = settingId;
			event.settingValue = settingValue;
			
			listener.onSettingChanged(event);
		}
	}
	
	public void addSettingListener(SettingListener listener)
	{
		m_settingListeners.add(listener);
	}
}
