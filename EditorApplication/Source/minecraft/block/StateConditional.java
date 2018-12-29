package minecraft.block;

public interface StateConditional
{
	public abstract boolean evaluate(BlockStateProvider state);
}
