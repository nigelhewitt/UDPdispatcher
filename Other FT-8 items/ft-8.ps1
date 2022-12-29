# run the FT-8 system
#Requires -RunAsAdministrator

# requires Install-Module VirtualDesktop to copy from repo to local PC
# see https://github.com/MScholtes/PSVirtualDesktop for function list

Get-Desktop 1 | Switch-Desktop

D:\Systems\wsjtx\bin\wsjtx.exe
D:\Systems\UDPdispatcher\x64\Release\UDPdispatcher.exe
D:\Nigel\Radio\Logging\GridTracker\GridTracker.exe
D:\Systems\VOACOP\Voaprop\VOAProp.exe

Sleep 4
Move-Mouse -x -1785 -y 46
Click-Mouse
Sleep 1
Move-Mouse -x -1712 -y 221
Click-Mouse

start-process -filepath "powershell" -Verb RunAs -Args D:\Util\ft-8-mon.ps1 `
  -WindowStyle Minimized
