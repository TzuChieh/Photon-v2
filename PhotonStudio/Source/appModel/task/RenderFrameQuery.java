package appModel.task;

import java.util.concurrent.atomic.AtomicInteger;

import appGui.EditorCtrl;
import appModel.project.Project;
import appView.RenderFrameView;
import javafx.application.Platform;
import javafx.concurrent.Task;
import photonApi.FrameRegion;
import photonApi.FrameStatus;
import photonApi.Ph;

public class RenderFrameQuery implements Runnable
{
	private Project m_project;
	private RenderFrameView m_view;
	
	public RenderFrameQuery(
		Project project,
		RenderFrameView view)
	{
		super();
		
		m_project = project;
		m_view = view;
	}

	@Override
	public void run()
	{
		FrameRegion updatedFrameRegion = new FrameRegion();
		FrameStatus frameStatus = m_project.asyncGetUpdatedFrame(Ph.ATTRIBUTE_LIGHT_ENERGY, updatedFrameRegion);
		if(frameStatus != FrameStatus.INVALID)
		{
			m_view.showIntermediate(updatedFrameRegion);
		}
	}
}
