package appModel.event;

public final class SettingEvent
{
	public Object source          = null;
	public int    settingId       = -1;
	public String oldSettingValue = "";
	public String newSettingValue = "";
}
