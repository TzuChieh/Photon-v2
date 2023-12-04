import os
import configparser


def get_setup_config():
	script_directory = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
	setup_config_path = os.path.join(script_directory, "../Main/SetupConfig.ini")
	setup_config = configparser.ConfigParser()
	setup_config.read_file(open(setup_config_path))
	return setup_config

def get_all_sections(config, prefix=""):
	sections = []
	for section_name in config.sections():
		if section_name.startswith(prefix):
			sections.append((section_name, config[section_name]))
	return sections

def get_all_projects(setup_config):
	return get_all_sections(setup_config, "Project.")

def get_project(setup_config, project_name):
	section_name = "Project." + project_name
	if setup_config.has_section(section_name):
		return (section_name, setup_config[section_name])
	else:
		print("Error: no project named \"%s\"" % project_name)
		return None
