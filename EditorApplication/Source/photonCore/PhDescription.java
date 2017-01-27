package photonCore;

import photonApi.LongRef;
import photonApi.Ph;

public final class PhDescription
{
	protected long m_descriptionId;
	
	public PhDescription()
	{
		LongRef descriptionId = new LongRef();
		Ph.phCreateDescription(descriptionId);
		m_descriptionId = descriptionId.m_value;
	}
	
	public void load(String filename)
	{
		Ph.phLoadDescription(m_descriptionId, filename);
	}
	
	public void update()
	{
		Ph.phUpdateDescription(m_descriptionId);
	}
	
	public void developFilm(PhFrame frame)
	{
		Ph.phDevelopFilm(m_descriptionId, frame.m_frameId);
	}
}
