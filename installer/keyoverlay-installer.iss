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
function InitializeSetup(): Boolean;
var
  OBSPath: string;
begin
  Result := True;
  if not RegQueryStringValue(HKLM, 'SOFTWARE\OBS Studio', '', OBSPath) and
     not RegQueryStringValue(HKCU, 'SOFTWARE\OBS Studio', '', OBSPath) then
  begin
    MsgBox('OBS Studio does not appear to be installed. Please install OBS Studio before installing KeyOverlay.', mbError, MB_OK);
    Result := False;
  end;
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then
  begin
    MsgBox('KeyOverlay installed! Restart OBS to activate the plugin.', mbInformation, MB_OK);
  end;
end;
