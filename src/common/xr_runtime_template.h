const char *runtime_default_template = R"_(
# If two runtimes are given the same name , the first
# valid one will be used. Items in this file are listed
# before the default items stored in the code. '~' will 
# be replaced with the HOME path on Linux.
#
# You can find the default configuration list on Github
# over here: https://github.com/maluoi/openxr-explorer/blob/main/src/common/xr_runtime_default.h
# Please feel free to make a PR, or raise an issue if you
# think a path should be added as a default!
#
# Format is:
# [OS id: windows|linux] [name, no space] [path to runtime manifest, spaces are ok]
#
# Here are some examples that are listed in the defaults:
# windows SteamVR C:\Program Files (x86)\Steam\steamapps\common\SteamVR\steamxr_win64.json
# linux SteamVR ~/.steam/steam/steamapps/common/SteamVR/steamxr_linux64.json

)_";