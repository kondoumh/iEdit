$ReleaseDir = "..\Release"
$ReleaseModule = "$ReleaseDir\iEdit.exe";
$DocDir = "..\doc"

$fileVersion = [Diagnostics.FileVersionInfo]::GetVersionInfo($ReleaseModule).FileVersion
$releaseVersion = [string]::Join("", ($fileVersion.Split('.') | Select-Object -First 3))
$archiveFile = "$ReleaseDir\iedit$releaseVersion.zip"

echo "File Version: $fileVersion"
echo $archiveFile

Compress-Archive -Path $ReleaseModule -DestinationPath $archiveFile -Force
Copy-Item -Path "..\README.md" -Destination "$ReleaseDir\README.txt"
Compress-Archive -Path "$ReleaseDir\README.txt" -DestinationPath $archiveFile -Update
Compress-Archive -Path "$DocDir\iEditDoc.iedx" -DestinationPath $archiveFile -Update
Compress-Archive -Path "$DocDir\albatross.ies" -DestinationPath $archiveFile -Update
Compress-Archive -Path "$DocDir\ui\" -DestinationPath $archiveFile -Update

$conf = Get-Content "$ReleaseDir\upload.conf" -Raw | ConvertFrom-StringData
$client = New-Object System.Net.WebClient
$client.Credentials = New-Object System.Net.NetworkCredential($conf.Item("user"), $conf.Item("pass"))
$hostname = $conf.Item("hostname")
$remotePath = $conf.Item("remotedir") + "iedit$releaseVersion.zip"
$client.BaseAddress = "ftp://" + $conf.Item("hostname") + $conf.Item("remoteroot") 
$client.UploadFile($remotePath, $archiveFile)
$client.Dispose()
