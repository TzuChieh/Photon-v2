package minecraft.block;

public interface StateConditional
{
	public abstract boolean evaluate(StateAggregate states);
}
