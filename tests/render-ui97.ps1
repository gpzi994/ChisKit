$ErrorActionPreference='Stop'
Add-Type -AssemblyName System.Drawing
$folder=Join-Path $PSScriptRoot '../build/ui97-preview'
foreach($f in Get-ChildItem $folder -Filter '*.ppm'){
 $b=[IO.File]::ReadAllBytes($f.FullName)
 $p=0;$lines=0
 while($lines -lt 3){if($b[$p++] -eq 10){$lines++}}
 $bmp=[Drawing.Bitmap]::new(320,288)
 try {
  for($y=0;$y -lt 288;$y++){for($x=0;$x -lt 320;$x++){
   $bmp.SetPixel($x,$y,[Drawing.Color]::FromArgb($b[$p],$b[$p+1],$b[$p+2]));$p+=3
  }}
  $bmp.Save([IO.Path]::ChangeExtension($f.FullName,'.png'),[Drawing.Imaging.ImageFormat]::Png)
 } finally {$bmp.Dispose()}
}
