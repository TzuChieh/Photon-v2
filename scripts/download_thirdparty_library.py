# download zipped library folder

from utility import downloader
from utility import console

import sys

git_branch_result = console.run_command("git", "branch")

# git branch returns branch names and indicate current branch like this
#
#   some-branch
#   some-branch2
# * some-branch3
#
# we need to parse it
#
asterisk_index = git_branch_result.index('*')
git_branch_name = git_branch_result[asterisk_index + 1:].strip()
print("Currently on %s branch" % git_branch_name)

# Use develop branch on third-party lib repo if we are not on master branch
if git_branch_name != "master":
    git_branch_name = "develop"
print("Third-party library version selected: %s" % git_branch_name)

src_filename = git_branch_name + ".zip"
src_file_url = "https://github.com/TzuChieh/Photon-v2-ThirdParty/archive/refs/heads/" + src_filename
dst_directory = sys.argv[1]

print("Downloading third-party libraries from <%s>..." % src_file_url)
downloader.download_zipfile_and_extract(src_file_url, dst_directory)
