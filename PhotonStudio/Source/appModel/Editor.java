package appModel;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.CancellationException;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Executor;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

import appModel.project.RenderProject;
import appModel.task.RenderFrameQuery;
import appModel.task.RenderStateEntry;
import appModel.task.RenderStatusQuery;
import appView.RenderFrameView;
import appView.RenderStatusView;
import javafx.application.Platform;
import javafx.concurrent.Task;
import photonApi.RenderState;
import photonApi.Statistics;
import util.Time;

public abstract class Editor extends ManagedResource
{
//	private RenderProject m_project;
//	private ExecutorService m_projectTaskExecutor;
//	private List<Future<?>> m_projectTaskHandles;
//	
//	private RenderStatusView m_renderStatusView;
//	private RenderFrameView m_renderFrameView;
//	private ScheduledExecutorService m_monitorService;
//	private List<Future<?>> m_monitorHandles;
//	
//	public Editor()
//	{
//		m_project = null;
//		m_projectTaskExecutor = null;
//		m_projectTaskHandles = new ArrayList<>();
//		
//		m_renderStatusView = new RenderStatusView(){};
//		m_renderFrameView = new RenderFrameView(){};
//		m_monitorService = null;
//		m_monitorHandles = new ArrayList<>();
//	}
//	
//	public RenderProject getProject()
//	{
//		return m_project;
//	}
//	
//	public void startMonitoring()
//	{
//		stopMonitoring();
//		awaitMonitoringStop();
//		
//		m_monitorHandles.add(
//			m_monitorService.scheduleWithFixedDelay(
//				newRenderStatusMonitor(), 
//				0, 1, TimeUnit.SECONDS));
//		
//		m_monitorHandles.add(
//			m_monitorService.scheduleWithFixedDelay(
//				new RenderFrameQuery(m_project, m_renderFrameView), 
//				0, 1, TimeUnit.SECONDS));
//	}
//	
//	public void stopMonitoring()
//	{
//		for(Future<?> monitorHandle : m_monitorHandles)
//		{
//			monitorHandle.cancel(true);
//		}
//	}
//	
//	public void awaitMonitoringStop()
//	{
//		for(Future<?> monitorHandle : m_monitorHandles)
//		{
//			try
//			{
//				monitorHandle.get();
//			}
//			catch(CancellationException e)
//			{
//				// cancellation of a monitor is valid, do nothing here
//			}
//			catch(Exception e)
//			{
//				e.printStackTrace();
//			}
//		}
//		
//		m_monitorHandles.clear();
//	}
//	
//	public void awaitProjectTasks()
//	{
//		for(Future<?> projectTaskHandle : m_projectTaskHandles)
//		{
//			try
//			{
//				projectTaskHandle.get();
//			}
//			catch(CancellationException e)
//			{
//				// cancellation of a task is valid, do nothing here
//			}
//			catch(Exception e)
//			{
//				e.printStackTrace();
//			}
//		}
//		
//		m_projectTaskHandles.clear();
//	}
//	
//	public void runLoadSceneTask()
//	{
//		Runnable loadSceneTask = m_project.newLoadSceneTask();
//		m_projectTaskHandles.add(m_projectTaskExecutor.submit(loadSceneTask));
//		
//		m_projectTaskHandles.add(m_projectTaskExecutor.submit(() -> startMonitoring()));
//	}
//	
//	public void runRenderTask()
//	{
//		Runnable renderTask = m_project.newRenderTask();
//		m_projectTaskHandles.add(m_projectTaskExecutor.submit(renderTask));
//		
//		m_projectTaskHandles.add(m_projectTaskExecutor.submit(() -> stopMonitoring()));
//	}
//	
//	public void runUpdateStaticImageTask()
//	{
//		Runnable developFilmTask = m_project.newUpdateStaticImageTask();
//		m_projectTaskHandles.add(m_projectTaskExecutor.submit(developFilmTask));
//		
//		m_projectTaskHandles.add(m_projectTaskExecutor.submit(() -> 
//		{
//			m_renderFrameView.showFinal(m_project.getLocalFinalFrame());
//		}));
//	}
//	
//	public boolean isProjectTaskRunning()
//	{
//		for(Future<?> projectTaskHandle : m_projectTaskHandles)
//		{
//			if(!projectTaskHandle.isDone())
//			{
//				return true;
//			}
//		}
//		return false;
//	}
//	
//	public void setProject(RenderProject project)
//	{
//		if(isProjectTaskRunning())
//		{
//			System.err.println("cannot set project while task is still running");
//			return;
//		}
//		
//		stopMonitoring();
//		awaitMonitoringStop();
//		
//		m_project = project;
//	}
//	
//	public void setRenderStatusView(RenderStatusView view)
//	{
//		m_renderStatusView = view;
//	}
//	
//	public void setRenderFrameView(RenderFrameView view)
//	{
//		m_renderFrameView = view;
//	}
//
//	@Override
//	protected void initResource()
//	{
//		m_projectTaskExecutor = Executors.newSingleThreadExecutor();
//		m_monitorService = Executors.newSingleThreadScheduledExecutor();
//	}
//
//	@Override
//	protected void freeResource()
//	{
//		awaitProjectTasks();
//		m_projectTaskExecutor.shutdown();
//		
//		stopMonitoring();
//		awaitMonitoringStop();
//		m_monitorService.shutdown();
//	}
//	
//	private Runnable newRenderStatusMonitor()
//	{
//		List<RenderStateEntry> states = new ArrayList<>();
//		for(int i = 0; i < 3; ++i)
//		{
//			RenderStateEntry integerState = RenderStateEntry.newInteger(m_project.getIntegerRenderStateName(i), i);
//			if(!integerState.getName().isEmpty())
//			{
//				states.add(integerState);
//			}
//			
//			RenderStateEntry realState = RenderStateEntry.newReal(m_project.getRealRenderStateName(i), i);
//			if(!realState.getName().isEmpty())
//			{
//				states.add(realState);
//			}
//		}
//		
//		return new RenderStatusQuery(m_project, states, m_renderStatusView);
//	}
}
