const char *runtime_default_list = R"_(
# If two runtimes are given the same name , the first
# valid one will be used. Items in this file are listed
# after the user's runtime list. ~ will be replaced 
# with the HOME path on Linux.
#
# Format is:
# [OS id: windows|linux] [name, no space] [path to runtime manifest, spaces are ok]
windows WMR C:\WINDOWS\system32\MixedRealityRuntime.json
windows Oculus C:\Program Files\Oculus\Support\oculus-runtime\oculus_openxr_64.json
windows SteamVR C:\Program Files (x86)\Steam\steamapps\common\SteamVR\steamxr_win64.json
windows Varjo C:\Program Files\Varjo\varjo-openxr\VarjoOpenXR.json
windows ViveOpenXR C:\Program Files (x86)\VIVE\Updater\App\ViveVRRuntime\ViveVR_openxr\ViveOpenXR.json
windows VirtualDesktopXRBundled C:\Program Files\Virtual Desktop Streamer\OpenXR\virtualdesktop-openxr-32.json
windows VirtualDesktopXRStandalone C:\Program Files\VirtualDesktopXR\virtualdesktop-openxr-32.json
linux Monado /usr/share/openxr/1/openxr_monado.json
linux SteamVR ~/.steam/steam/steamapps/common/SteamVR/steamxr_linux64.json)_";
