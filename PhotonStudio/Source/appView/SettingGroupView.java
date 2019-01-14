package appView;

public interface SettingGroupView
{
	default void showSetting(String name, String value)
	{}
	
	default void showSetting(String name, String oldValue, String newValue)
	{}
}
