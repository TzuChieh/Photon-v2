package appGui.renderProject;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicInteger;

import appModel.renderProject.RenderFrameQuery;
import appModel.renderProject.RenderProject;
import appModel.renderProject.RenderStateEntry;
import appModel.renderProject.RenderStatusQuery;
import appModel.renderProject.RenderStatusView;
import javafx.application.Platform;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.collections.FXCollections;
import javafx.concurrent.Service;
import javafx.concurrent.Task;
import javafx.fxml.FXML;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.Label;
import javafx.scene.control.ProgressBar;
import javafx.scene.layout.ColumnConstraints;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.Priority;
import javafx.scene.layout.VBox;
import photonApi.FrameRegion;
import photonApi.FrameStatus;
import photonApi.Ph;
import photonApi.RenderState;
import photonApi.Statistics;
import util.Time;

public class RenderStatusCtrl
{
	@FXML private VBox              variablesVBox;
	@FXML private ProgressBar       renderProgressBar;
	@FXML private Label             percentageProgressLabel;
	@FXML private Label             timeRemainingLabel;
	@FXML private Label             timeSpentLabel;
	@FXML private ChoiceBox<String> attributeChoiceBox;
	
	private AtomicInteger m_chosenAttribute;
	private int m_numVBoxBaseChildren;
	
	private RenderStatusView m_renderStatusView;
	
	public RenderStatusCtrl()
	{
		m_chosenAttribute = new AtomicInteger(Ph.ATTRIBUTE_LIGHT_ENERGY);
		
		m_numVBoxBaseChildren = 0;
	}
	
	@FXML
    public void initialize()
	{
    	attributeChoiceBox.setItems(FXCollections.observableArrayList(
    		"Light Energy",
    		"Normal"
    	));
    	attributeChoiceBox.getSelectionModel().select("Light Energy");
    	attributeChoiceBox.getSelectionModel().selectedIndexProperty().addListener(new ChangeListener<Number>()
		{
			@Override
			public void changed(ObservableValue<? extends Number> observable, Number oldValue, Number newValue)
			{
				m_chosenAttribute.set(newValue.intValue());
				System.err.println(m_chosenAttribute.get());
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
	
	public RenderStatusView getRenderStatusView()
	{
		return m_renderStatusView;
	}
}
