import logging
import os
import sys
import shutil
import tempfile

import bbmp_subprocess
import bbmp_util as util


def rel_to_py(*paths):
    return os.path.join(os.path.realpath(os.path.dirname(__file__)), *paths)


logger = logging.getLogger(__name__)
logger.setLevel(logging.INFO)

PROJECT_DIR = rel_to_py("..")

VCVARSALL_COMMAND = []
PLATFORM_SPECIFIC_CMAKE_PARAMETERS = []

if sys.platform == "win32":
    vcvarsall = util.get_most_recent_file(
        util.find_file(
            "vcvarsall.bat",
            root="C:\\",
            search_constraint=["Program Files", "Visual Studio"],
            cache_result=False,
        )
    )
    assert vcvarsall, "vcvarsall.bat not found"

    VCVARSALL_COMMAND = [vcvarsall, "x86_amd64"]
    PLATFORM_SPECIFIC_CMAKE_PARAMETERS = ["-G", "NMake Makefiles"]

if __name__ == "__main__":
    logging.basicConfig()

    install_dir = rel_to_py("..", "install")

    if not os.path.exists(install_dir):
        os.makedirs(install_dir)

    with tempfile.TemporaryDirectory() as cmake_build_dir:
        commands = [
            VCVARSALL_COMMAND,
            [
                "cmake",
                f"-DCMAKE_INSTALL_PREFIX={install_dir}",
                "-DCMAKE_BUILD_TYPE=Release",
                "-S",
                PROJECT_DIR,
                "-B",
                cmake_build_dir,
                *PLATFORM_SPECIFIC_CMAKE_PARAMETERS,  # selects generator on Windows
            ],
            [
                "cmake",
                "--build",
                cmake_build_dir,
                "--config",
                "Release",
                "--target",
                "install",
            ],
        ]
        bbmp_subprocess.run_in_shell(commands)

    # Prepare files in order the user has InnoSetup and wants to create an installer
    shutil.copy(
        os.path.join(install_dir, "Bebump Broitness.exe"),
        os.path.join(install_dir, "broitness.exe"),
    )
    shutil.copy(
        rel_to_py("..", "installer", "broitness.iss"),
        os.path.join(install_dir),
    )
