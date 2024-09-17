import subprocess
import sys


def run_command(command_name, *arguments):
	return run_command_from("./", command_name, *arguments)

def run_command_from(working_dir, command_name, *arguments):
	"""
	Execute a console command from a specific directory. Errors (stderr), if any,
	are printed out automatically.
	@param working_dir The working directory to run the command from.
	@return The output (stdout) from the command.
	"""
	command_args = [command_name]
	command_args.extend([arg for arg in arguments])

	# capture_output will make stdout and stderr both captured (separately)
	command_result = subprocess.run(
		command_args, 
		cwd=working_dir, 
		capture_output=True, 
		text=True)

	out_str = command_result.stdout
	err_str = command_result.stderr

	if command_result.returncode != 0:
		print(
			f"command <{", ".join([str(arg) for arg in command_args])}> ran with error "
			f"(error code: {command_result.returncode}), details: {err_str}")
		
	return out_str

def run_python(working_dir, *arguments):
	return run_python_from("./", *arguments)

def run_python_from(working_dir, *arguments):
	command_name = 'python'
	if sys.platform == 'linux':
		command_name = 'python3'

	return run_command_from(working_dir, command_name, *arguments)
