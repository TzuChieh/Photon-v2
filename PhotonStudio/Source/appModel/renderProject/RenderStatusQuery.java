package appModel.renderProject;

import java.util.ArrayList;
import java.util.List;

import javafx.application.Platform;
import photonApi.ObservableRenderData;
import photonApi.PhEngine;
import photonApi.RenderState;
import photonApi.Statistics;
import util.Time;

/**
 * A query that can keep track on states of the engine, and show the results
 * using an associated view.
 */
public class RenderStatusQuery implements Runnable
{
	private PhEngine               m_engine;
	private RenderStatusView       m_view;
	
	private List<RenderStateEntry> m_states;
	private String[]               m_names;
	private String[]               m_values;
	private Statistics             m_statistics;
	private double                 m_startTimeMs;
	
	public RenderStatusQuery(PhEngine engine, RenderStatusView view)
	{
		super();
		
		m_engine = engine;
		m_view   = view;
		
		// retrieve and determine which states to show
		
		ObservableRenderData data = m_engine.getObservableRenderData();
		
		m_states = new ArrayList<>();
		for(int i = 0; i < data.integerNames.length; ++i)
		{
			RenderStateEntry integerState = RenderStateEntry.newInteger(data.integerNames[i], i);
			if(!integerState.getName().isEmpty())
			{
				m_states.add(integerState);
			}
		}
		for(int i = 0; i < data.realNames.length; ++i)
		{
			RenderStateEntry realState = RenderStateEntry.newReal(data.realNames[i], i);
			if(!realState.getName().isEmpty())
			{
				m_states.add(realState);
			}
		}
		
		m_names  = new String[m_states.size()];
		m_values = new String[m_states.size()];
		for(int i = 0; i < m_names.length; ++i)
		{
			m_names[i]  = m_states.get(i).getName();
			m_values[i] = "";
		}
		
		m_statistics  = new Statistics();
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
		m_engine.asyncGetRendererStatistics(m_statistics);
		
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
		
		// TODO: use Number, abstract away long & float
		RenderState state = m_engine.asyncGetRenderState();
		for(int i = 0; i < m_states.size(); ++i)
		{
			RenderStateEntry entry = m_states.get(i);
			if(entry.isInteger())
			{
				m_values[i] = Long.toString(state.integerStates[entry.getIndex()]);
			}
			else if(entry.isReal())
			{
				m_values[i] = Float.toString(state.realStates[entry.getIndex()]);
			}
			else
			{
				m_values[i] = "invalid";
			}
		}
		
		Platform.runLater(() -> 
		{
			m_view.showStates(m_names, m_values);
		});
	}
}
