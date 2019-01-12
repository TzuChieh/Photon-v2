package appGui.projectMonitor;

import java.util.concurrent.atomic.AtomicInteger;

import appGui.EditorCtrl;
import appModel.project.Project;
import javafx.application.Platform;
import javafx.concurrent.Task;
import photonApi.FrameRegion;
import photonApi.FrameStatus;

public class RenderFrameQueryTask extends Task<Void>
{
	private Project m_project;
	
	private EditorCtrl    display;
	private AtomicInteger chosenAttribute;
	
	public RenderFrameQueryTask(
		Project project,
		EditorCtrl display,
		AtomicInteger chosenAttribute)
	{
		super();
		
		m_project = project;
		
		this.display = display;
		this.chosenAttribute = chosenAttribute;
	}
	
	@Override
	protected Void call() throws Exception
	{
		while(!isCancelled())
		{
			
			// TODO: need to add these monitoring attributes to a project's data, 
			// otherwise other finished projects (with work done = 100%) will cause
			// this loop to break (thus not updating GUI anymore until rendering is finished)
			
			
			FrameRegion updatedFrameRegion = new FrameRegion();
			FrameStatus frameStatus = m_project.asyncGetUpdatedFrame(chosenAttribute.get(), updatedFrameRegion);
			if(frameStatus != FrameStatus.INVALID)
			{
				Platform.runLater(() ->
				{
					display.loadFrameBuffer(updatedFrameRegion);
					display.drawFrame();
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
		}// end while
		
		return null;
	}
}
