# the purpose of ft-8-mon.ps1 is to monitor wsjt-x for a 'Log QSO' dialog and
# then go 'Bong' to wake me up.

Sleep 5
$bong = $false
while($true){
	$p = Get-Process | where {$_.name -match 'wsjtx' }
	if($p -eq $null){ break }
	if($p.MainWindowTitle -match 'Log QSO'){
		if(-not $bong){
			[System.Media.SystemSounds]::Asterisk.Play()
			$bong = $true
		}
	}
	else{
		$bong = $false;
	}
	Sleep 1
}

# start with:
#  start-process -filepath "powershell" -Verb RunAs `
#  -Args D:\Util\ft-8-mon.ps1 -WindowStyle Minimized