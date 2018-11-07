package appGui;

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
import javafx.scene.layout.GridPane;
import photonApi.FrameRegion;
import photonApi.FrameStatus;
import photonApi.Ph;
import photonApi.RenderState;
import photonApi.Statistics;
import util.Time;

public class RenderProgressMonitorCtrl
{
	@FXML private GridPane          variableGridPane;
	@FXML private ProgressBar       renderProgressBar;
	@FXML private Label             percentageProgressLabel;
	@FXML private Label             spsLabel;
	@FXML private Label             timeRemainingLabel;
	@FXML private Label             timeSpentLabel;
	@FXML private ChoiceBox<String> attributeChoiceBox;
	
	private volatile boolean m_isMonitoring;
	private AtomicInteger    m_chosenAttribute;
	private Service<Void>    m_monitorService;
	
	private Project    m_project;
	private EditorCtrl m_display;
	
	public RenderProgressMonitorCtrl()
	{
		m_isMonitoring    = false;
		m_chosenAttribute = new AtomicInteger(Ph.ATTRIBUTE_LIGHT_ENERGY);
		m_monitorService  = genMonitorService();
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
						spsLabel.setText(Long.toString((long)statistics.samplesPerSecond));
						timeSpentLabel.setText((long)(renderTimeMs / 1000.0) + " s");
						timeRemainingLabel.setText((long)(remainingRenderTimeMs / 1000.0) + " s");
					});
					
					// HACK
					RenderState state = m_project.asyncGetRenderState();
					for(int i = 0; i < 3; ++i)
					{
						System.out.println("integer state " + i + " = " + state.integerStates[i]);
					}
					for(int i = 0; i < 3; ++i)
					{
						System.out.println("real state " + i + " = " + state.realStates[i]);
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
				
				m_isMonitoring = false;
				
				return null;
			}
		};
	}
}
