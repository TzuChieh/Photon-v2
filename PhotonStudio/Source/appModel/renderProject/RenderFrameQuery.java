package appModel.renderProject;

import java.util.concurrent.atomic.AtomicInteger;

import appGui.renderProject.RenderProjectCtrl;
import javafx.application.Platform;
import javafx.concurrent.Task;
import photonApi.FrameRegion;
import photonApi.FrameStatus;
import photonApi.Ph;

public class RenderFrameQuery implements Runnable
{
	private RenderProject m_project;
	private RenderFrameView m_view;
	
	public RenderFrameQuery(
		RenderProject project,
		RenderFrameView view)
	{
		super();
		
		m_project = project;
		m_view = view;
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
		FrameRegion updatedFrameRegion = new FrameRegion();
		FrameStatus frameStatus = m_project.asyncGetUpdatedFrame(Ph.ATTRIBUTE_LIGHT_ENERGY, updatedFrameRegion);
		if(frameStatus != FrameStatus.INVALID)
		{
			Platform.runLater(() -> 
			{
				m_view.showIntermediate(updatedFrameRegion);
			});
		}
	}
}
