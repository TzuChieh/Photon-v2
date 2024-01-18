render_engine_id_name = 'PHOTON'
exporter_engine_id_name = 'PHOTON_EXPORT'
cycles_engine_id_name   = 'CYCLES'

photon_engines = {render_engine_id_name, exporter_engine_id_name}
photon_and_cycles_engines = photon_engines | {cycles_engine_id_name}
