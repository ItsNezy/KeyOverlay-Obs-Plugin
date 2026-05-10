[Setup]
AppName=KeyOverlay for OBS
AppVersion=1.0.0
DefaultDirName={pf}\obs-studio
DefaultGroupName=KeyOverlay
OutputDir=Output
OutputBaseFilename=keyoverlay-installer
Compression=lzma
SolidCompression=yes
PrivilegesRequiredOverridesAllowed=dialog

[Files]
Source: "..\build_x64\RelWithDebInfo\keyoverlay.dll"; DestDir: "{pf}\obs-studio\obs-plugins\64bit\"
Source: "..\data\*"; DestDir: "{pf}\obs-studio\data\obs-plugins\keyoverlay\"; Flags: recursesubdirs

[Code]
procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then
  begin
    MsgBox('KeyOverlay installed! Restart OBS to activate the plugin.', mbInformation, MB_OK);
  end;
end;
