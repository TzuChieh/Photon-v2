import os
import configparser


def get_setup_config():
	script_directory = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
	setup_config_path = os.path.join(script_directory, "../Main/SetupConfig.ini")
	setup_config = configparser.ConfigParser()
	setup_config.read_file(open(setup_config_path))
	return setup_config

def get_all_sections(config, prefix=""):
	"""
	@return A list of tuples, each tuple contains name of the section and the actual section.
	"""
	sections = []
	for section_name in config.sections():
		if section_name.startswith(prefix):
			sections.append((section_name, config[section_name]))
	return sections

def get_all_projects(setup_config):
	"""
	@return A list of tuples, each tuple contains (section) name of the project and the actual section.
	"""
	return get_all_sections(setup_config, "Project.")

def get_project(setup_config, project_name):
	"""
	@return A tuple that contains name of the section and the actual section.
	@exception ValueError If there is no project named `project_name`.
	"""
	section_name = "Project." + project_name
	if setup_config.has_section(section_name):
		return (section_name, setup_config[section_name])
	else:
		raise ValueError("no project named \"%s\"" % project_name)
