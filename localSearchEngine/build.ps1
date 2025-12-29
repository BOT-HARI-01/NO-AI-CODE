
param(
    [string]$file
)
$vcbat = "D:\Microsoft Visual Studio\VC\Auxiliary\Build\vcvars64.bat"
$project = "D:\CODEPRACTICE\NO-AI-WORK\localSearchEngine"

cmd /c "`"$vcbat`" && cd /d $project && cl /std:c++17 /EHsc $file"