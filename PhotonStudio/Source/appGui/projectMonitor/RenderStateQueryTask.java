package appGui.projectMonitor;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.atomic.AtomicInteger;

import appGui.EditorCtrl;
import appModel.project.Project;
import javafx.application.Platform;
import javafx.concurrent.Task;
import javafx.fxml.FXML;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.Label;
import javafx.scene.control.ProgressBar;
import javafx.scene.layout.VBox;
import photonApi.FrameRegion;
import photonApi.FrameStatus;
import photonApi.RenderState;
import photonApi.Statistics;
import util.Time;

public class RenderStateQueryTask extends Task<Void>
{
	private Project m_project;
	private List<RenderStateEntry> m_states;
	
	private Label             percentageProgressLabel;
	private Label             timeRemainingLabel;
	private Label             timeSpentLabel;
	
	public RenderStateQueryTask(
		Project project,
		List<RenderStateEntry> states,
		Label             percentageProgressLabel,
		Label             timeRemainingLabel,
		Label             timeSpentLabel)
	{
		super();
		
		m_project = project;
		m_states = states;
		
		this.percentageProgressLabel = percentageProgressLabel;
		this.timeRemainingLabel = timeRemainingLabel;
		this.timeSpentLabel = timeSpentLabel;
	}
	
	@Override
	protected Void call() throws Exception
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
		
		return null;
	}
}
