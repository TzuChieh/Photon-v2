package appGui;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.atomic.AtomicInteger;

import appGui.util.RenderStateEntry;
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

public class RenderStateMonitorCtrl
{
	@FXML private VBox              variablesVBox;
	@FXML private ProgressBar       renderProgressBar;
	@FXML private Label             percentageProgressLabel;
	@FXML private Label             timeRemainingLabel;
	@FXML private Label             timeSpentLabel;
	@FXML private ChoiceBox<String> attributeChoiceBox;
	
	private volatile boolean m_isMonitoring;
	private AtomicInteger    m_chosenAttribute;
	private Service<Void>    m_monitorService;
	
	private ArrayList<RenderStateEntry> m_states;
	private int m_numVBoxBaseChildren;
	
	private Project    m_project;
	private EditorCtrl m_display;
	
	public RenderStateMonitorCtrl()
	{
		m_isMonitoring    = false;
		m_chosenAttribute = new AtomicInteger(Ph.ATTRIBUTE_LIGHT_ENERGY);
		m_monitorService  = genMonitorService();
		
		m_states = new ArrayList<>();
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
    	
    	renderProgressBar.progressProperty().bind(m_monitorService.progressProperty());
    	
    	m_numVBoxBaseChildren = variablesVBox.getChildren().size();
	}
	
	public void startMonitoring()
	{
		// we should not start until previous one has stopped
		waitForStopMonitoring();
		
		updateStateNames();
		m_isMonitoring = true;
		m_monitorService.restart();
	}
	
	public void stopMonitoring()
	{
		m_monitorService.cancel();
	}
	
	public void waitForStopMonitoring()
	{
		stopMonitoring();
		
		while(m_isMonitoring)
		{}
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
	
	private Service<Void> genMonitorService()
	{
		return new Service<Void>()
		{
			@Override
			protected Task<Void> createTask()
			{
				return genMonitorTask();
			}
		};
	}
	
	private Task<Void> genMonitorTask()
	{
		return new Task<Void>()
		{
			@Override
			protected Void call()
			{
				final double     renderStartMs = Time.getTimeMs();
				final Statistics statistics    = new Statistics();
				while(!isCancelled())
				{
					m_project.asyncGetRendererStatistics(statistics);
					
					final long workDone  = (long)(statistics.percentageProgress + 0.5f);
					final long totalWork = 100;
					updateProgress(workDone, totalWork);
					
					final double workDoneFraction      = statistics.percentageProgress / 100.0;
					final double renderTimeMs          = Time.getTimeMs() - renderStartMs;
					final double totalRenderTimeMs     = renderTimeMs / workDoneFraction;
					final double remainingRenderTimeMs = totalRenderTimeMs * (1.0 - workDoneFraction);
					
					Platform.runLater(() -> 
					{
						percentageProgressLabel.setText(Float.toString(statistics.percentageProgress));
						timeSpentLabel.setText((long)(renderTimeMs / 1000.0) + " s");
						timeRemainingLabel.setText((long)(remainingRenderTimeMs / 1000.0) + " s");
					});
					
					RenderState state = m_project.asyncGetRenderState();
					for(RenderStateEntry entry : m_states)
					{
						String stringValue = "invalid";
						if(entry.isInteger())
						{
							stringValue = Long.toString(state.integerStates[entry.getIndex()]);
						}
						else if(entry.isReal())
						{
							stringValue = Float.toString(state.realStates[entry.getIndex()]);
						}
						
						String text = stringValue;
						Platform.runLater(() -> 
						{
							entry.getValueLabel().setText(text);
						});
					}
					
					// TODO: need to add these monitoring attributes to a project's data, 
					// otherwise other finished projects (with work done = 100%) will cause
					// this loop to break (thus not updating GUI anymore until rendering is finished)
					
					if(workDone >= totalWork)
					{
						Platform.runLater(() -> percentageProgressLabel.setText("100"));
						break;
					}
					
					FrameRegion updatedFrameRegion = new FrameRegion();
					FrameStatus frameStatus = m_project.asyncGetUpdatedFrame(m_chosenAttribute.get(), updatedFrameRegion);
					if(frameStatus != FrameStatus.INVALID)
					{
						Platform.runLater(() ->
						{
							m_display.loadFrameBuffer(updatedFrameRegion);
							m_display.drawFrame();
						});
					}
					
					try
					{
						Thread.sleep(1000);
					}
					catch(InterruptedException e)
					{
						if(!isCancelled())
						{
							e.printStackTrace();
						}
						
						break;
					}
				}// end while(true)
				
				Platform.runLater(() -> percentageProgressLabel.setText("100"));
				updateProgress(100, 100);
				
				// TODO: update other statistics for the last time
				
				m_isMonitoring = false;
				
				return null;
			}
		};
	}
	
	private void updateStateNames()
	{
		while(variablesVBox.getChildren().size() != m_numVBoxBaseChildren)
		{
			variablesVBox.getChildren().remove(variablesVBox.getChildren().size() - 1);
		}
		m_states.clear();
		
		for(int i = 0; i < 3; ++i)
		{
			RenderStateEntry integerState = RenderStateEntry.newInteger(m_project.getIntegerRenderStateName(i), i);
			if(!integerState.isEmpty())
			{
				m_states.add(integerState);
			}
			
			RenderStateEntry realState = RenderStateEntry.newReal(m_project.getRealRenderStateName(i), i);
			if(!realState.isEmpty())
			{
				m_states.add(realState);
			}
		}
		
		ColumnConstraints stateLayout = new ColumnConstraints();
		stateLayout.setPercentWidth(50.0);
		stateLayout.setHgrow(Priority.ALWAYS);
		
		for(RenderStateEntry state : m_states)
		{
			GridPane cell = new GridPane();
			cell.add(state.getNameLabel(),  0, 0);
			cell.add(state.getValueLabel(), 1, 0);
            cell.getColumnConstraints().add(stateLayout);
			variablesVBox.getChildren().add(cell);
		}
	}
}
