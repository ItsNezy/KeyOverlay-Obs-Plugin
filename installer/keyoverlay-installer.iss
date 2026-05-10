[Setup]
AppName=KeyOverlay for OBS
AppVersion=1.0.0
AppPublisher=KeyOverlay
DefaultGroupName=KeyOverlay
OutputDir=Output
OutputBaseFilename=keyoverlay-installer
Compression=lzma
SolidCompression=yes
PrivilegesRequired=admin
PrivilegesRequiredOverridesAllowed=dialog
; We set the install dir dynamically in InitializeSetup
DefaultDirName={commonpf64}\obs-studio
DisableDirPage=no

[Files]
; Plugin DLL
Source: "files\obs-plugins\64bit\keyoverlay.dll"; DestDir: "{app}\obs-plugins\64bit"; Flags: ignoreversion
; Plugin data (web UI, locale, etc)
Source: "files\data\obs-plugins\keyoverlay\*"; DestDir: "{app}\data\obs-plugins\keyoverlay"; Flags: ignoreversion recursesubdirs createallsubdirs

[UninstallDelete]
Type: filesandsubdirs; Name: "{app}\data\obs-plugins\keyoverlay"

[Code]
function GetOBSInstallPath(): string;
var
  Path: string;
begin
  Result := '';
  if RegQueryStringValue(HKLM, 'SOFTWARE\OBS Studio', '', Path) then
    Result := Path
  else if RegQueryStringValue(HKCU, 'SOFTWARE\OBS Studio', '', Path) then
    Result := Path;
end;

function InitializeSetup(): Boolean;
var
  OBSPath: string;
begin
  OBSPath := GetOBSInstallPath();
  if (OBSPath = '') and (not DirExists(ExpandConstant('{commonpf64}\obs-studio'))) then
  begin
    MsgBox('OBS Studio does not appear to be installed.' + #13#10 +
           'Please install OBS Studio first, then run this installer again.', mbError, MB_OK);
    Result := False;
  end else
    Result := True;
end;

procedure InitializeWizard();
var
  OBSPath: string;
begin
  OBSPath := GetOBSInstallPath();
  if OBSPath <> '' then
    WizardForm.DirEdit.Text := OBSPath;
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then
  begin
    MsgBox('KeyOverlay installed successfully!' + #13#10 +
           'Please restart OBS Studio to activate the plugin.', mbInformation, MB_OK);
  end;
end;
