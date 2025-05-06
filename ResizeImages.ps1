 param (
    [string]$directoryPath,
    [string]$resizePercent
 )


$files = Get-ChildItem -Path $directoryPath -File

foreach ($file in $files) {
    # Access the file's full path
    $filePath = $file.FullName
    
    Write-Output $filePath
    $resizeArg = "$($resizePercent)%"

    magick $filePath -resize $resizeArg $filePath
}