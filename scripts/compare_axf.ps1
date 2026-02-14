param(
    [Parameter(Mandatory = $true)]
    [string]$OldAxf,

    [Parameter(Mandatory = $true)]
    [string]$NewAxf,

    [string]$OutputDir,

    [string]$FromElfPath
)

$ErrorActionPreference = 'Stop'

function Resolve-FromElfPath {
    param([string]$Hint)

    if ($Hint) {
        if (Test-Path $Hint) { return (Resolve-Path $Hint).Path }
        throw "Specified fromelf path not found: $Hint"
    }

    $cmd = Get-Command fromelf -ErrorAction SilentlyContinue
    if ($cmd) { return $cmd.Source }

    $candidates = @(
        'C:\Users\cyang\AppData\Local\Keil_v5\ARM\ARMCLANG\bin\fromelf.exe',
        'C:\Keil_v5\ARM\ARMCLANG\bin\fromelf.exe',
        'C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe'
    )

    foreach ($candidate in $candidates) {
        if (Test-Path $candidate) { return $candidate }
    }

    throw 'fromelf.exe not found. Install Keil/ArmClang or pass -FromElfPath.'
}

function Invoke-FromElf {
    param(
        [string]$Exe,
        [string[]]$Args,
        [string]$StepName
    )

    Write-Host "[fromelf] $StepName"
    & $Exe @Args
    if ($LASTEXITCODE -ne 0) {
        throw "fromelf failed at step: $StepName"
    }
}

function Invoke-FromElfText {
    param(
        [string]$Exe,
        [string[]]$Args,
        [string]$OutFile,
        [string]$StepName
    )

    Write-Host "[fromelf] $StepName"
    & $Exe @Args | Out-File -FilePath $OutFile -Encoding utf8
    if ($LASTEXITCODE -ne 0) {
        throw "fromelf failed at step: $StepName"
    }
}

function Write-TextDiff {
    param(
        [string]$OldFile,
        [string]$NewFile,
        [string]$OutFile
    )

    $git = Get-Command git -ErrorAction SilentlyContinue
    if ($git) {
        & git --no-pager diff --no-index -- "$OldFile" "$NewFile" *> "$OutFile"
    }
    else {
        Compare-Object (Get-Content $OldFile) (Get-Content $NewFile) -IncludeEqual:$false |
        Out-File -FilePath $OutFile -Encoding utf8
    }
}

function Get-FirstBinaryDiffOffset {
    param(
        [byte[]]$A,
        [byte[]]$B
    )

    $min = [Math]::Min($A.Length, $B.Length)
    for ($i = 0; $i -lt $min; $i++) {
        if ($A[$i] -ne $B[$i]) { return $i }
    }

    if ($A.Length -ne $B.Length) { return $min }
    return -1
}

$oldPath = (Resolve-Path $OldAxf).Path
$newPath = (Resolve-Path $NewAxf).Path

if (-not $OutputDir) {
    $timestamp = Get-Date -Format 'yyyyMMdd_HHmmss'
    $OutputDir = Join-Path (Get-Location) "axf_compare_$timestamp"
}

$OutputDir = [System.IO.Path]::GetFullPath($OutputDir)
New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null

$fromelf = Resolve-FromElfPath -Hint $FromElfPath

Write-Host "Old AXF: $oldPath"
Write-Host "New AXF: $newPath"
Write-Host "fromelf: $fromelf"
Write-Host "Output : $OutputDir"

$oldText = Join-Path $OutputDir 'old_text.txt'
$newText = Join-Path $OutputDir 'new_text.txt'
$oldDis = Join-Path $OutputDir 'old_disasm.txt'
$newDis = Join-Path $OutputDir 'new_disasm.txt'
$oldBin = Join-Path $OutputDir 'old.bin'
$newBin = Join-Path $OutputDir 'new.bin'
$textDiff = Join-Path $OutputDir 'diff_text.patch'
$disDiff = Join-Path $OutputDir 'diff_disasm.patch'
$summary = Join-Path $OutputDir 'summary.txt'

Write-Host '[fromelf] old text/section export'
& $fromelf --text -z $oldPath | Out-File -FilePath $oldText -Encoding utf8
if ($LASTEXITCODE -ne 0) { throw 'fromelf failed at step: old text/section export' }

Write-Host '[fromelf] new text/section export'
& $fromelf --text -z $newPath | Out-File -FilePath $newText -Encoding utf8
if ($LASTEXITCODE -ne 0) { throw 'fromelf failed at step: new text/section export' }

Write-Host '[fromelf] old disassembly export'
& $fromelf --text -c $oldPath | Out-File -FilePath $oldDis -Encoding utf8
if ($LASTEXITCODE -ne 0) { throw 'fromelf failed at step: old disassembly export' }

Write-Host '[fromelf] new disassembly export'
& $fromelf --text -c $newPath | Out-File -FilePath $newDis -Encoding utf8
if ($LASTEXITCODE -ne 0) { throw 'fromelf failed at step: new disassembly export' }
Write-Host '[fromelf] old binary export'
& $fromelf --output $oldBin --bin $oldPath
if ($LASTEXITCODE -ne 0) { throw 'fromelf failed at step: old binary export' }

Write-Host '[fromelf] new binary export'
& $fromelf --output $newBin --bin $newPath
if ($LASTEXITCODE -ne 0) { throw 'fromelf failed at step: new binary export' }

Write-Host '[diff] generating text and disassembly patches'
Write-TextDiff -OldFile $oldText -NewFile $newText -OutFile $textDiff
Write-TextDiff -OldFile $oldDis -NewFile $newDis -OutFile $disDiff

$oldHash = Get-FileHash $oldBin -Algorithm SHA256
$newHash = Get-FileHash $newBin -Algorithm SHA256
$oldBytes = [System.IO.File]::ReadAllBytes($oldBin)
$newBytes = [System.IO.File]::ReadAllBytes($newBin)
$firstDiff = Get-FirstBinaryDiffOffset -A $oldBytes -B $newBytes

@(
    "old_axf=$oldPath"
    "new_axf=$newPath"
    "fromelf=$fromelf"
    "old_bin_size=$($oldBytes.Length)"
    "new_bin_size=$($newBytes.Length)"
    "old_bin_sha256=$($oldHash.Hash)"
    "new_bin_sha256=$($newHash.Hash)"
    "first_binary_diff_offset=$firstDiff"
    "same_binary=$([bool]($oldHash.Hash -eq $newHash.Hash))"
) | Out-File -FilePath $summary -Encoding utf8

Write-Host "Done. Review:"
Write-Host "  $summary"
Write-Host "  $textDiff"
Write-Host "  $disDiff"
