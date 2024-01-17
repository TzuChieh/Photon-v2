from psdl import sdl


def to_filter_enum(b_filter_type):
	filter_name = None
	if b_filter_type == 'BOX':
		filter_name = "box"
	elif b_filter_type == 'GAUSSIAN':
		filter_name = "gaussian"
	elif b_filter_type == 'MN':
		filter_name = "mn"
	elif b_filter_type == 'BH':
		filter_name = "bh"
	else:
		print("warning: unsupported filter type %s, using box instead" % b_filter_type)
		filter_name = "box"

	return sdl.Enum(filter_name)

def to_integrator_enum(b_integrator_type):
	integrator_name = None
	if b_integrator_type == 'BVPT':
		integrator_name = "bvpt"
	elif b_integrator_type == 'BNEEPT':
		integrator_name = "bneept"
	elif b_integrator_type == 'BVPTDL':
		integrator_name = 'bvptdl'
	elif b_integrator_type == 'VPM':
		integrator_name = "vanilla"
	elif b_integrator_type == 'PPM':
		integrator_name = "progressive"
	elif b_integrator_type == 'SPPM':
		integrator_name = "stochastic-progressive"
	elif b_integrator_type == 'PPPM':
		integrator_name = "probabilistic-progressive"
	else:
		print("warning: unsupported integrator type %s, using bvpt instead" % b_integrator_type)
		integrator_name = "bvpt"

	return sdl.Enum(integrator_name)

def to_scheduler_enum(b_scheduler_type):
	scheduler_name = None
	if b_scheduler_type == 'BULK':
		scheduler_name = "bulk"
	elif b_scheduler_type == 'STRIPE':
		scheduler_name = "stripe"
	elif b_scheduler_type == 'GRID':
		scheduler_name = "grid"
	elif b_scheduler_type == 'TILE':
		scheduler_name = "tile"
	elif b_scheduler_type == 'SPIRAL':
		scheduler_name = "spiral"
	elif b_scheduler_type == 'SPIRAL_GRID':
		scheduler_name = "spiral-grid"
	else:
		print("warning: unsupported scheduler type %s, using bulk instead" % b_scheduler_type)
		scheduler_name = "bulk"

	return sdl.Enum(scheduler_name)
