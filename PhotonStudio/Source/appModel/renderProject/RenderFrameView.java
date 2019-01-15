package appModel.renderProject;

import photonApi.Frame;
import photonApi.FrameRegion;

public interface RenderFrameView
{
	default void showIntermediate(FrameRegion frame)
	{}
	
	default void showFinal(Frame frame)
	{}
}
