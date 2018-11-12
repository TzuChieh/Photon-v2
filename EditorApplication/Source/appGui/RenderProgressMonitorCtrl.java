package appGui;

import java.util.ArrayList;
import java.util.concurrent.atomic.AtomicInteger;

import appModel.project.Project;
import javafx.application.Platform;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.collections.FXCollections;
import javafx.concurrent.Service;
import javafx.concurrent.Task;
import javafx.concurrent.Worker;
import javafx.fxml.FXML;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.Label;
import javafx.scene.control.ProgressBar;
import javafx.scene.layout.ColumnConstraints;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Priority;
import javafx.scene.layout.VBox;
import javafx.scene.text.Text;
import photonApi.FrameRegion;
import photonApi.FrameStatus;
import photonApi.Ph;
import photonApi.RenderState;
import photonApi.Statistics;
import util.Time;

public class RenderProgressMonitorCtrl
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
	
	public ArrayList<String> m_integerNames;
	public ArrayList<String> m_realNames;
	public ArrayList<Label>  m_integers;
	public ArrayList<Label>  m_reals;
	
	private Project    m_project;
	private EditorCtrl m_display;
	
	public RenderProgressMonitorCtrl()
	{
		m_isMonitoring    = false;
		m_chosenAttribute = new AtomicInteger(Ph.ATTRIBUTE_LIGHT_ENERGY);
		m_monitorService  = genMonitorService();
		
		m_integerNames = new ArrayList<>();
		m_realNames    = new ArrayList<>();
		m_integers     = new ArrayList<>();
		m_reals        = new ArrayList<>();
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
	}
	
	public void startMonitoring()
	{
		// we should not start until previous one has stopped
		waitForStopMonitoring();
		
		gatherMonitoredVariables();
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
					for(int i = 0; i < 3; ++i)
					{
						if(!m_integerNames.get(i).isEmpty())
						{
							Label  label = m_integers.get(i);
							String value = Long.toString(state.integerStates[i]);
							Platform.runLater(() -> 
							{
								label.setText(value);
							});
						}
					}
					for(int i = 0; i < 3; ++i)
					{
						if(!m_realNames.get(i).isEmpty())
						{
							Label  label = m_reals.get(i);
							String value = Float.toString(state.realStates[i]);
							Platform.runLater(() -> 
							{
								label.setText(value);
							});
						}
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
	
	private void gatherMonitoredVariables()
	{
		ColumnConstraints colLayout = new ColumnConstraints();
		colLayout.setPercentWidth(50.0);
		colLayout.setHgrow(Priority.ALWAYS);
		
		m_integerNames.clear();
		for(int i = 0; i < 3; ++i)
		{
			String name  = m_project.getIntegerRenderStateName(i);
			Label  label = new Label();
			
			m_integerNames.add(name);
			m_integers.add(label);
			
			if(!name.isEmpty())
			{
				GridPane cell = new GridPane();
				cell.add(new Label(name), 0, 0);
				cell.add(label, 1, 0);
	            cell.getColumnConstraints().add(colLayout);
				variablesVBox.getChildren().add(cell);
			}
		}
		
		m_realNames.clear();
		for(int i = 0; i < 3; ++i)
		{
			String name  = m_project.getRealRenderStateName(i);
			Label  label = new Label();
			
			m_realNames.add(name);
			m_reals.add(label);
			
			if(!name.isEmpty())
			{
				GridPane cell = new GridPane();
				cell.add(new Label(name), 0, 0);
				cell.add(label, 1, 0);
	            cell.getColumnConstraints().add(colLayout);
				variablesVBox.getChildren().add(cell);
			}
		}
	}
}
