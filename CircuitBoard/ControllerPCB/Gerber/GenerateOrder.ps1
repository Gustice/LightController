## Powershell script for renamin KiCad Files according to SeedStudio rules.

$ProjectName = "Controller"
$NewProjectName = "TeaCandle"

# Sets working path to location of script
Push-Location (Split-Path -Path $MyInvocation.MyCommand.Definition -Parent)

Remove-Item "PCB_Order" -Recurse -ErrorAction Ignore
Remove-Item "PCB_Order.zip" -Recurse -ErrorAction Ignore

# Creates a new directory in this path
New-Item -ItemType Directory -Path "PCB_Order"


# Copying existing KiCad-Fils in created directory
# .GTO     Top Silkscreen
# .GTS     Top Soldermask
# .GTL     Top Copper
# .GBL     Bottom Copper
# .GBS     Bottom Soldermask
# .GBO     Bottom Silkscreen
# .DRL     Drilling-File
# Es muss nur die gm1-Endung umbenannt werden
# .gm1 => .GML     Milling File

$files = @("F.SilkS.gto","F.Mask.gts","F.Cu.gtl","B.Cu.gbl","B.Mask.gbs","B.SilkS.gbo")
foreach($fe in $files) {
    $afe = $fe.Substring($fe.Length - 4,4)
    Copy-Item "$ProjectName-$fe" "PCB_Order\$NewProjectName$afe"
}

Copy-Item "$ProjectName.drl" "PCB_Order\$NewProjectName.drl"
Copy-Item "$ProjectName-Edge.Cuts.gm1" "PCB_Order\$NewProjectName.gml"

Compress-Archive -Path "PCB_Order" -DestinationPath "$NewProjectName-PCB_Order.zip" -CompressionLevel NoCompression
