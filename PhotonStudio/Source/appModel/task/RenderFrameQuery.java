package appModel.task;

import java.util.concurrent.atomic.AtomicInteger;

import appGui.EditorCtrl;
import appModel.project.RenderProject;
import appView.RenderFrameView;
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
		// FIXME: try-catch all
		
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
