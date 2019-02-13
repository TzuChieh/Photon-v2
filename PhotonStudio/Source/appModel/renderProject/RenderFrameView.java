package appModel.renderProject;

import photonApi.Frame;
import photonApi.FrameRegion;
import photonApi.FrameStatus;

public interface RenderFrameView
{
	default void showPeeked(FrameRegion frame, FrameStatus status)
	{}
	
	default void showFinal(Frame frame)
	{}
}
