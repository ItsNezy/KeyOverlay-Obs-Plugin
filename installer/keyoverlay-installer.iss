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
Source: "..\build\keyoverlay.dll"; DestDir: "{app}\obs-plugins\64bit\"; Flags: ignoreversion
Source: "..\data\*"; DestDir: "{app}\data\obs-plugins\keyoverlay\"; Flags: ignoreversion recursesubdirs createallsubdirs

[Code]
procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then
  begin
    MsgBox('KeyOverlay installed! Restart OBS to activate the plugin.', mbInformation, MB_OK);
  end;
end;
