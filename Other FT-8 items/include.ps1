# useful things to be included by the profiles

# add Nigel's Library
Import-Module D:\Systems\PowerShell\Library\cmdlet1\bin\Debug\cmdlet1.dll

# enhanced prompt

function Prompt {   
    Write-Host "PS $(Get-Location)" -ForegroundColor Green -NoNewline
    if($NestedPromptLevel -ge 1) { Write-Host '>>' -ForegroundColor White -NoNewline }
    Write-Host '>' -ForegroundColor White -NoNewline
    return " "
}

# additional dir wide command

function dirw {
    param ($x)
    (@(Get-ChildItem -Directory $x -Name | % { Write-Output "$_\" }) `
    + @(Get-ChildItem -File $x -Name | % { Write-Output "$_" }) `
    | Format-Wide {$_} -AutoSize -Force | Out-String) -Replace '(\r\n)+', "`r`n"
}

# cursor/mouse stuff

# put the cursor in a specific absolute screen position
# (works with my three screens)

function Move-Mouse {
	param([int]$x, [int]$y)
	Add-Type -AssemblyName System.Windows.Forms
	$screen = [System.Windows.Forms.SystemInformation]::VirtualScreen
	($screen | Get-Member -MemberType Property) | Out-Null			# prevent screen rubbish
	$screen.Width = $x
	$screen.Height = $y
	[Windows.Forms.Cursor]::Position = "$($screen.Width),$($screen.Height)"
}

# read and print the 'current' cursor position

function Get-Mouse {
	Add-Type -AssemblyName System.Windows.Forms
	$position = [Windows.Forms.Cursor]::Position
	Write-Host "x = " $position.x " y = " $position.y
}

# 'press' a mouse button -key left(default) right middle
#  WARNING WARNING requires Run as Administrator for some programs
  
function Click-Mouse {
	param($key)
$Mouse=@' 
[DllImport("user32.dll",CharSet=CharSet.Auto, CallingConvention=CallingConvention.StdCall)]
public static extern void mouse_event(long dwFlags, long dx, long dy, long cButtons, long dwExtraInfo);
'@ 
	$SendMouseClick = Add-Type -memberDefinition $Mouse -name "Win32MouseEventNew" -namespace Win32Functions -passThru

	if($key -like 'r*'){		# right
		$down = 0x08
		$up   = 0x10
	}
	elseif($key -like 'm*'){	# middle
		$down = 0x20
		$up   = 0x40
	}
	else {						# left and default
		$down = 0x2				# MOUSEEVENTF_LEFTDOWN
		$up   = 0x4				# MOUSEEVENTF_LEFTUP
	}
	$SendMouseClick::mouse_event($down,0,0,0,0);
	$SendMouseClick::mouse_event($up,0,0,0,0);
}

# locate a program running on the PC and setup its window coordinates

function Find-Program {
	param($name)

	Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;

public static class DesktopWindows {
    public struct Rect {
       public int Left { get; set; }
       public int Top { get; set; }
       public int Right { get; set; }
       public int Bottom { get; set; }
    }

    [DllImport("user32.dll")]
    public static extern bool GetWindowRect(IntPtr hwnd, ref Rect rectangle);
}
'@

	$processesByName = [System.Diagnostics.Process]::GetProcessesByName($name)

	foreach($process in $processesByName){
	    if($process.MainWindowHandle -ne 0){
	        $windowRect = [DesktopWindows+Rect]::new()
	        $return = [DesktopWindows]::GetWindowRect($process.MainWindowHandle,[ref]$windowRect)
	            if($return){
	                [PSCustomObject]@{ProcessName=$name; ProcessID=$process.Id; MainWindowHandle=$process.MainWindowHandle; WindowTitle=$process.MainWindowTitle; Top=$windowRect.Top; Left=$windowRect.Left;}
	            }
	            else{
	                echo 'Failed to get window rect'
	            }
	    }
	    else{
	        echo "No MainWindowHandle for $name"
	    }
	}
}

# move to the next desktop in the sequence

function Next-Desktop {
	$i = (Get-CurrentDesktop | Get-DesktopIndex)
	$n = Get-DesktopCount
	$i++
	if($i -eq $n){ $i = 0; }
	Get-Desktop $i | Switch-Desktop
}

 