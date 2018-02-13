$ReleaseDir = "..\Release"
$ReleaseModule = $ReleaseDir + "\iEdit.exe";

$arr = ([System.Diagnostics.FileVersionInfo]::GetVersionInfo($ReleaseModule).FileVersion).Split('.') | Select-Object -First 3
$ReleaseVersion = [string]::Join("", $arr)

echo $ReleaseVersion
