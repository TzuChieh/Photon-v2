from utility import downloader
from utility import console
from utility import filesystem

import sys
import os
import configparser


# Download third-party libraries for the engine
def download_thirdparty_library(dst_directory, setup_config: configparser.ConfigParser):

    final_folder_path = os.path.join(dst_directory, "Photon-v2-ThirdParty")

    # Delete old library folder first if it exists (basically a clean install)
    if filesystem.delete_folder_with_contents(final_folder_path):
        print("Old library folder deleted")

    git_branch_result = console.run_command("git", "branch")

    # "git branch" returns branch names and indicate current branch like this
    #
    #   some-branch
    #   some-branch2
    # * some-branch3
    #
    # we need to parse it
    #
    git_branch_tokens = git_branch_result.split()
    asterisk_index = git_branch_tokens.index("*")
    photon_branch_name = git_branch_tokens[asterisk_index + 1]
    print("Currently on %s branch" % photon_branch_name)

    settings = setup_config["General"]

    # Only the main Photon branch uses the main lib branch. For other branches, use the dev lib branch.
    # The lib branch to use can be overridden by directly specifying a custom branch to use.
    lib_branch_name = settings.get("CustomThirdPartyBranch", "")
    if not lib_branch_name:
        if photon_branch_name == settings["MainBranch"]:
            lib_branch_name = settings["MainThirdPartyBranch"]
        else:
            lib_branch_name = settings["DevThirdPartyBranch"]
    print("Third-party library version selected: %s" % lib_branch_name)

    src_filename = lib_branch_name + ".zip"
    src_file_url = "https://github.com/TzuChieh/Photon-v2-ThirdParty/archive/refs/heads/" + src_filename

    print("Downloading third-party libraries from <%s>..." % src_file_url)
    downloader.download_zipfile_and_extract(src_file_url, dst_directory)

    # The extracted zip file will be a folder named "Photon-v2-ThirdParty-<branch-name>",
    # rename it to be just "Photon-v2-ThirdParty"
    extracted_folder_path = os.path.join(dst_directory, "Photon-v2-ThirdParty-" + lib_branch_name)
    filesystem.rename_folder(extracted_folder_path, final_folder_path)

    # Check resources
    if os.path.isdir(final_folder_path):
        print("Third-party libraries saved to <%s>" % final_folder_path)
    else:
        print("Failed to locate third-party libraries, expected to be <%s>" % final_folder_path, file=sys.stderr)
