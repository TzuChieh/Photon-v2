from abc import ABC, abstractmethod


class SdlFragment(ABC):

	def __init__(self):
		super(SdlFragment, self).__init__()

	@abstractmethod
	def to_sdl_fragment(self):
		pass


class SdlCommand(ABC):

	def __init__(self):
		super(SdlCommand, self).__init__()

	@abstractmethod
	def to_sdl(self):
		pass
