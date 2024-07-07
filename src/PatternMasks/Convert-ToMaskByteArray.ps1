param
(
    $inFile,
    $outFile
)

$bmp = [System.Drawing.Bitmap]::FromFile((Resolve-Path $inFile).ProviderPath)
if(-not (Test-Path $outFile)) { New-Item $outFile}
$preprocessorFileName =(Get-Item (Resolve-Path $outFile).ProviderPath).BaseName.Replace('.', '_').ToUpper()

$output = "#ifndef $preprocessorFileName`n"
$output += "#define $preprocessorFileName`n`n"
$output += "#define $($preprocessorFileName)_ANIMATIONFRAMES $($bmp.Height)`n"
$output += "#define $($preprocessorFileName)_ANIMATIONRESOLUTION $($bmp.Width)`n`n"
$output += "uint8_t maskFrames[$($preprocessorFileName)_ANIMATIONFRAMES][$($preprocessorFileName)_ANIMATIONRESOLUTION] =`n{`n"
$lines = @()
foreach($h in 1..$bmp.Height){
    $line = "`t{"
    $bytes = @()
    foreach($w in 1..$bmp.Width) {
        $value = 255 - $bmp.getPixel($w-1, $h-1).R
        
        $bytes += $value.ToString().PadLeft([Math]::Log10(256-$value) + 1, ' ')
    }
    $line += $bytes -join ","
    $line += " }"
    $lines += $line
}
$output += $lines -join ",`n"
$output += "`n}"
$output += "`n#endif"
$output | Out-File $outFile