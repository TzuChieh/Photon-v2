package photonCore;

import photonApi.LongRef;
import photonApi.Ph;

public final class PhFilm
{
	protected long m_filmId;
	
	public PhFilm(int widthPx, int heightPx)
	{
		LongRef filmId = new LongRef();
		Ph.phCreateFilm(filmId, widthPx, heightPx);
		m_filmId = filmId.m_value;
	}
	
	public void develop(PhFrame frame)
	{
		Ph.phDevelopFilm(m_filmId, frame.m_frameId);
	}
}
