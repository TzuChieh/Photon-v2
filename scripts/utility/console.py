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
		raise ValueError(
			f"command <{', '.join([str(arg) for arg in command_args])}> ran with error "
			f"(error code: {command_result.returncode}), details: {err_str}")
		
	return out_str

def run_python(*arguments):
	"""
	Run Python from current working directory. Interpreter will be the one that runs this script.
	"""
	return run_python_from("./", *arguments)

def run_python_from(working_dir, *arguments):
	"""
	Run Python from `working_dir`. Interpreter will be the one that runs this script.
	"""
	# Use the same interpreter that runs current script. If we simply call `python`,
	# we may accidently run the system python in virtual environment.
	command_name = sys.executable
	return run_command_from(working_dir, command_name, *arguments)
