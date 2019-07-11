package appGui;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.cell.PropertyValueFactory;
import javafx.util.Callback;

public class ToolboxCtrl
{
	@FXML private TableView<Tool>           toolsTableView;
	@FXML private TableColumn<Tool, String> toolNameTableColumn;
	@FXML private TableColumn<Tool, String> toolDescriptionTableColumn;
	
	private Callback<String, Void> m_toolNameCallback;
	private ObservableList<Tool>   m_tools;
	
	public ToolboxCtrl()
	{
		m_toolNameCallback = new Callback<String, Void>()
		{
			@Override
			public Void call(String toolName)
			{
				return null;
			}
		};
		
		m_tools = FXCollections.observableArrayList();
	}
	
	@FXML
    public void initialize()
    {
		toolsTableView.setItems(m_tools);
		toolNameTableColumn.setCellValueFactory(new PropertyValueFactory<>("name"));
		toolDescriptionTableColumn.setCellValueFactory(new PropertyValueFactory<>("description"));
    }
	
	@FXML
	void openSelectedBtnClicked(ActionEvent event)
	{
		final Tool selectedTool = toolsTableView.getSelectionModel().getSelectedItem();
		if(selectedTool != null)
		{
			assert(m_toolNameCallback != null);
			
			m_toolNameCallback.call(selectedTool.getName());
		}
	}
	
	public void addTool(String name, String description)
	{
		m_tools.add(new Tool(name, description));
	}
	
	public void setToolNameCallback(Callback<String, Void> callback)
	{
		m_toolNameCallback = callback;
	}
	
	public static class Tool
	{
		private String m_name;
		private String m_description;
		
		public Tool(String name, String description)
		{
			m_name        = name;
			m_description = description;
		}
		
		public String getName()
		{
			return m_name;
		}
		
		public String getDescription()
		{
			return m_description;
		}
		
		@Override
		public String toString()
		{
			return "tool name: " + m_name + ", tool description: " + m_description;
		}
	}
}
