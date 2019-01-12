package appGui.projectMonitor;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicInteger;

import appGui.EditorCtrl;
import appModel.project.Project;
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

public class ProjectMonitorCtrl
{
	@FXML private VBox              variablesVBox;
	@FXML private ProgressBar       renderProgressBar;
	@FXML private Label             percentageProgressLabel;
	@FXML private Label             timeRemainingLabel;
	@FXML private Label             timeSpentLabel;
	@FXML private ChoiceBox<String> attributeChoiceBox;
	
	
	private Service<Void> m_renderStateQueryService;
	private Service<Void> m_renderFrameQueryService;
	
	private AtomicInteger m_chosenAttribute;
	private int m_numVBoxBaseChildren;
	
	private Project    m_project;
	private EditorCtrl m_display;
	
	public ProjectMonitorCtrl()
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
    	
    	m_renderStateQueryService = new Service<Void>()
		{
			@Override
			protected Task<Void> createTask()
			{
				return newRenderStateQueryTask();
			}
		};
		m_renderFrameQueryService = new Service<Void>()
		{
			@Override
			protected Task<Void> createTask()
			{
				return newRenderFrameQueryTask();
			}
		};
		
		// we should monitor single project at a time
		m_renderStateQueryService.setExecutor(Executors.newSingleThreadExecutor());
		m_renderFrameQueryService.setExecutor(Executors.newSingleThreadExecutor());
		
		renderProgressBar.progressProperty().bind(m_renderStateQueryService.progressProperty());
	}
	
	public void startMonitoring()
	{
		m_renderStateQueryService.restart();
		m_renderFrameQueryService.restart();
	}
	
	public void stopMonitoring()
	{
		m_renderStateQueryService.cancel();
		m_renderFrameQueryService.cancel();
	}
	
	public void setMonitoredProject(Project project)
	{
		m_project = project;
	}
	
	// TODO: use a dedicated display class
	public void setDisplay(EditorCtrl display)
	{
		m_display = display;
	}
	
	private Task<Void> newRenderStateQueryTask()
	{
		// TODO: states should use a dedicated vbox, so that it can simply be cleared here
		while(variablesVBox.getChildren().size() != m_numVBoxBaseChildren)
		{
			variablesVBox.getChildren().remove(variablesVBox.getChildren().size() - 1);
		}
		
		List<RenderStateEntry> states = new ArrayList<>();
		for(int i = 0; i < 3; ++i)
		{
			RenderStateEntry integerState = RenderStateEntry.newInteger(m_project.getIntegerRenderStateName(i), i);
			if(!integerState.getNameLabel().getText().isEmpty())
			{
				states.add(integerState);
			}
			
			RenderStateEntry realState = RenderStateEntry.newReal(m_project.getRealRenderStateName(i), i);
			if(!realState.getNameLabel().getText().isEmpty())
			{
				states.add(realState);
			}
		}
		
		ColumnConstraints stateLayout = new ColumnConstraints();
		stateLayout.setPercentWidth(50.0);
		stateLayout.setHgrow(Priority.ALWAYS);
		
		for(RenderStateEntry state : states)
		{
			GridPane cell = new GridPane();
			cell.add(state.getNameLabel(),  0, 0);
			cell.add(state.getValueLabel(), 1, 0);
            cell.getColumnConstraints().add(stateLayout);
			variablesVBox.getChildren().add(cell);
		}
		
		return new RenderStateQueryTask(m_project, states, percentageProgressLabel, timeRemainingLabel, timeSpentLabel);
	}
	
	private Task<Void> newRenderFrameQueryTask()
	{
		return new RenderFrameQueryTask(m_project, m_display, m_chosenAttribute);
	}
}
