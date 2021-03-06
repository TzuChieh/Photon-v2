package appGui.renderProject;

import java.util.concurrent.atomic.AtomicInteger;

import appModel.renderProject.RenderProject;
import appModel.renderProject.RenderStatusView;
import javafx.application.Platform;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.collections.FXCollections;
import javafx.fxml.FXML;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.Label;
import javafx.scene.control.ProgressBar;
import javafx.scene.layout.ColumnConstraints;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.Priority;
import javafx.scene.layout.VBox;
import photonApi.Ph;

public class RenderStatusCtrl
{
	// TODO: able to select target attribute to display
	
	@FXML private VBox               variablesVBox;
	@FXML private ProgressBar        renderProgressBar;
	@FXML private Label              percentageProgressLabel;
	@FXML private Label              timeRemainingLabel;
	@FXML private Label              timeSpentLabel;
	@FXML private ChoiceBox<Integer> channelIndexChoiceBox;
	
	private RenderStatusView m_renderStatusView;
	private RenderProject    m_renderProject;
	
	private int m_numVBoxBaseChildren;
	
	public RenderStatusCtrl()
	{
		m_numVBoxBaseChildren = 0;
	}
	
	@FXML
    public void initialize()
	{
    	channelIndexChoiceBox.setItems(FXCollections.observableArrayList(
    		0,
    		1
    	));
    	channelIndexChoiceBox.getSelectionModel().select(0);
    	channelIndexChoiceBox.getSelectionModel().selectedIndexProperty().addListener(new ChangeListener<Number>()
		{
			@Override
			public void changed(ObservableValue<? extends Number> observable, Number oldValue, Number newValue)
			{
				m_renderProject.setMonitoredChannel(newValue.intValue());
				
				// DEBUG
				System.err.println(newValue.intValue());
			}
		});
    	
    	m_numVBoxBaseChildren = variablesVBox.getChildren().size();
		
		m_renderStatusView = new RenderStatusView()
		{
			private Label[] m_names;
			private Label[] m_values;
			
			@Override
			public void showProgress(float normalizedProgress)
			{
				Platform.runLater(() -> 
				{
					renderProgressBar.setProgress(normalizedProgress);
					percentageProgressLabel.setText(Float.toString(normalizedProgress * 100));
				});
			}
			
			@Override
			public void showTimeRemaining(long ms)
			{
				Platform.runLater(() -> timeRemainingLabel.setText(ms / 1000 + " s"));
			}
			
			@Override
			public void showTimeSpent(long ms)
			{
				Platform.runLater(() -> timeSpentLabel.setText(ms / 1000 + " s"));
			}
			
			@Override
			public void showStates(String[] names, String[] values)
			{
				Platform.runLater(() -> 
				{
					// TODO: states should use a dedicated vbox, so that it can simply be cleared here
					if(variablesVBox.getChildren().size() != m_numVBoxBaseChildren + names.length)
					{
						while(variablesVBox.getChildren().size() != m_numVBoxBaseChildren)
						{
							variablesVBox.getChildren().remove(variablesVBox.getChildren().size() - 1);
						}
						
						m_names = new Label[names.length];
						m_values = new Label[values.length];
						
						ColumnConstraints stateLayout = new ColumnConstraints();
						stateLayout.setPercentWidth(50.0);
						stateLayout.setHgrow(Priority.ALWAYS);
						
						for(int i = 0; i < names.length; ++i)
						{
							m_names[i] = new Label();
							m_values[i] = new Label();
							
							GridPane cell = new GridPane();
							cell.add(m_names[i],  0, 0);
							cell.add(m_values[i], 1, 0);
				            cell.getColumnConstraints().add(stateLayout);
							variablesVBox.getChildren().add(cell);
						}
					}
					
					for(int i = 0; i < names.length; ++i)
					{
						m_names[i].setText(names[i]);
						m_values[i].setText(values[i]);
					}
				});
			}
		};
	}
	
	public void setRenderProject(RenderProject project)
	{
		m_renderProject = project;
	}
	
	public RenderStatusView getView()
	{
		return m_renderStatusView;
	}
}
