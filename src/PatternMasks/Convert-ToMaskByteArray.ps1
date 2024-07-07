param
(
    $inFile,
    $outFile
)
Add-Type -Assembly System.Drawing

if(-not (Test-Path $inFile)) { Write-Error "File not found: $inFile"; exit}


$bmp = [System.Drawing.Bitmap]::FromFile((Resolve-Path $inFile).ProviderPath)
if(-not (Test-Path $outFile)) { New-Item $outFile}
$safeName = (Get-Item (Resolve-Path $outFile).ProviderPath).BaseName.Replace('.', '_')
$preprocessorFileName = $safeName.ToUpper()

$output = "#ifndef $preprocessorFileName`n"
$output += "#define $preprocessorFileName`n`n"
$output += "#include `"PatternMaskSource.h`"`n`n"
$output += "#define $($safeName)_ANIMATIONFRAMES $($bmp.Height)`n"
$output += "#define $($safeName)_ANIMATIONRESOLUTION $($bmp.Width)`n`n"

$output += @'
class SAFENAME_Mask : public PatternMaskSource
{
public:
	SAFENAME_Mask() : PatternMaskSource()
	{
	}

protected:
	int getMask(int frame, int pixel)
	{
		return maskFrames[frame][pixel % SAFENAME_ANIMATIONRESOLUTION];
	}

	int getAnimationFrames()
	{
		return SAFENAME_ANIMATIONFRAMES;
	}

	int getAnimationResolution()
	{
		return SAFENAME_ANIMATIONRESOLUTION;
	}

private:
'@.Replace("SAFENAME", $safeName)

$output += "`n`tint maskFrames[$($safeName)_ANIMATIONFRAMES][$($safeName)_ANIMATIONRESOLUTION] =`n{`n"
$lines = @()
foreach($h in 1..$bmp.Height){
    $line = "`t`t{"
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
$output += "`n`t};`n"
$output += "};`n"
$output += "`n#endif"
$output | Out-File $outFile