render_engine_idname = 'PHOTON'
exporter_engine_idname = 'PHOTON_EXPORT'
cycles_engine_idname   = 'CYCLES'

photon_engines = {render_engine_idname, exporter_engine_idname}
photon_and_cycles_engines = photon_engines | {cycles_engine_idname}
