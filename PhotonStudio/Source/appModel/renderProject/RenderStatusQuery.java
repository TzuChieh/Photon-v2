package appModel.renderProject;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.atomic.AtomicInteger;

import appGui.renderProject.RenderProjectCtrl;
import javafx.application.Platform;
import javafx.concurrent.Task;
import javafx.fxml.FXML;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.Label;
import javafx.scene.control.ProgressBar;
import javafx.scene.layout.VBox;
import photonApi.FrameRegion;
import photonApi.FrameStatus;
import photonApi.Ph;
import photonApi.RenderState;
import photonApi.Statistics;
import util.Time;

public class RenderStatusQuery implements Runnable
{
	private RenderProject m_project;
	private List<RenderStateEntry> m_states;
	private RenderStatusView m_view;
	
	private String[] m_names;
	private String[] m_values;
	private Statistics m_statistics;
	private double m_startTimeMs;
	
	public RenderStatusQuery(
		RenderProject project,
		List<RenderStateEntry> states,
		RenderStatusView view)
	{
		super();
		
		m_project = project;
		m_states = states;
		m_view = view;
		
		m_names = new String[states.size()];
		for(int i = 0; i < m_names.length; ++i)
		{
			m_names[i] = states.get(i).getName();
		}
		
		m_values = new String[states.size()];
		
		m_statistics = new Statistics();
		m_startTimeMs = Time.getTimeMs();
	}

	@Override
	public void run()
	{
		// Scheduled executors will silently consumes errors and stop running,
		// at least we can get an error message by this try-catch block.
		try
		{
			query();
		}
		catch(Throwable t)
		{
			t.printStackTrace();
		}
	}
	
	private void query()
	{
		m_project.asyncGetRendererStatistics(m_statistics);
		
		final double normalizedProgress    = m_statistics.percentageProgress / 100.0;
		final double renderTimeMs          = Time.getTimeMs() - m_startTimeMs;
		final double totalRenderTimeMs     = renderTimeMs / normalizedProgress;
		final double remainingRenderTimeMs = totalRenderTimeMs * (1.0 - normalizedProgress);
		
		Platform.runLater(() -> 
		{
			m_view.showProgress((float)normalizedProgress);
			m_view.showTimeSpent((long)(renderTimeMs + 0.5));
			m_view.showTimeRemaining((long)(remainingRenderTimeMs + 0.5));
		});
		
		RenderState state = m_project.asyncGetRenderState();
		for(RenderStateEntry entry : m_states)
		{
			String value = "invalid";
			if(entry.isInteger())
			{
				value = Long.toString(state.integerStates[entry.getIndex()]);
			}
			else if(entry.isReal())
			{
				value = Float.toString(state.realStates[entry.getIndex()]);
			}
			
			m_values[entry.getIndex()] = value;
		}
		
		Platform.runLater(() -> 
		{
			m_view.showStates(m_names, m_values);
		});
	}
}
