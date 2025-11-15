param([string]$FilePath)
(Get-Content $FilePath) -replace '#include "cpr/(.*)"', '#include "`$1"' | Set-Content $FilePath