; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "qwtw library"
#define MyAppVersion "1.0.3"
#define MyAppPublisher "Igor Sandler"
#define MyAppURL "https://github.com/ig-or/qwtw"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{4558C8F9-BC53-4ECC-A15F-98084EDB413B}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\qwtw
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
OutputBaseFilename=qwtwsetup-win-x64-1.0.2
Compression=lzma
SolidCompression=yes
ArchitecturesInstallIn64BitMode=x64

#define tch "vs2015-x64"
#define boostver "1.62.0"
#define qtver "5.7.0"

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]

Source: "{#SourcePath}..\lib-{#tch}\release\qwtwc.lib"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SourcePath}..\qwtw\c_lib\qwtw_c.h"; DestDir: "{app}"; Flags: ignoreversion

Source: "{#SourcePath}..\lib-{#tch}\release\qwtwc.dll"; DestDir: "{commonappdata}/qwtw";    Check: Is64BitInstallMode;
Source: "{#SourcePath}..\lib-{#tch}\release\qwtwtest.exe"; DestDir: "{commonappdata}/qwtw"; Flags: ignoreversion

Source: "{#SourcePath}vs-redist\{#tch}\*";   DestDir: "{commonappdata}/qwtw";    Check: Is64BitInstallMode;

Source: "{#SourcePath}..\..\extlib\marble\1.11.3\{#tch}\bin\astro.dll"; DestDir: "{commonappdata}/qwtw"; Check: Is64BitInstallMode; Flags: 
Source: "{#SourcePath}..\..\extlib\marble\1.11.3\{#tch}\bin\marblewidget-qt5.dll"; DestDir: "{commonappdata}/qwtw"; Check: Is64BitInstallMode; Flags: ignoreversion
Source: "{#SourcePath}..\..\extlib\marble\1.11.3\data\*"; DestDir: "{commonappdata}/qwtw/marble-data";  Flags: ignoreversion    recursesubdirs

;Source: "{#SourcePath}..\..\extlib\openssl\1.1.0c\{#tch}\bin\*"; DestDir: "{commonappdata}/qwtw";      Flags: ignoreversion    recursesubdirs 
Source: "{#SourcePath}..\..\extlib\openssl\1.0.2f\vs2013-x64\bin\*"; DestDir: "{commonappdata}/qwtw";      Flags: ignoreversion    recursesubdirs 

Source: "{#SourcePath}..\..\extlib\boost\{#boostver}\{#tch}\*"; DestDir: "{commonappdata}/qwtw"; Check: Is64BitInstallMode; Flags:  recursesubdirs
Source: "{#SourcePath}..\..\extlib\qt\{#qtver}\{#tch}\*"; DestDir: "{commonappdata}/qwtw";  Check: Is64BitInstallMode; Flags: recursesubdirs

Source: "{#SourcePath}..\..\extlib\qwt\6.1.4\{#tch}\bin\qwt.dll"; DestDir: "{commonappdata}/qwtw"; Flags: ignoreversion


[Icons]
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"

[InstallDelete]
;Type: filesandordirs; Name: "{commonappdata}/rdframer"
Type: filesandordirs; Name: "{%TEMP}/qwtw"
Type: filesandordirs; Name: "{commonappdata}/qwtw"

[UninstallDelete]
;Type: filesandordirs; Name: "{commonappdata}/rdframer"
Type: filesandordirs; Name: "{%TEMP}/qwtw"
Type: filesandordirs; Name: "{commonappdata}/qwtw"


