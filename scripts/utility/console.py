import subprocess


def run_command(command_name, *arguments):
	return run_command_from("./", command_name, *arguments)

def run_command_from(working_directory, command_name, *arguments):

	"""
	Execute a console command from a specific directory. Errors (stderr), if any,
	are printed out automatically.
	@return The output (stdout) from the command.
	"""

	command_args = [command_name]
	command_args.extend([arg for arg in arguments])

	# capture_output will make stdout and stderr both captured (separately)
	command_result = subprocess.run(
		command_args, 
		cwd=working_directory, 
		capture_output=True, 
		text=True)

	output_str = command_result.stdout
	error_str = command_result.stderr

	if error_str:
		print("command <%s> ran with error: %s" % 
			(", ".join([str(arg) for arg in command_args]), error_str))
		
	return output_str
