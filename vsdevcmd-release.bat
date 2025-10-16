call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
msbuild ".\\"%1".vcxproj" /p:configuration=release
cd App
".\\"%1".exe"
