@echo off

:: Look for machine-wide Chrome installs (stable, Beta, and Dev).
:: Get the name, running version (if an update is pending relaunch), and
:: installed version of each.
FOR %%A IN (
    {8A69D345-D564-463c-AFF1-A69D9E530F96},
    {8237E44A-0054-442C-B6B6-EA0509993955},
    {401C381F-E0DE-4B85-8BD8-3F3F14FBDA57}) DO (
  reg query "HKLM\Software\Google\Update\Clients\%%A" /v name "/reg:32" 2>NUL
  reg query "HKLM\Software\Google\Update\Clients\%%A" /v opv "/reg:32" 2>NUL
  reg query "HKLM\Software\Google\Update\Clients\%%A" /v pv "/reg:32" 2>NUL
)

:: Look for Chrome installs in the current user's %LOCALAPPDATA% directory
:: (stable, Beta, Dev, and canary).
:: Get the name, running version (if an update is pending relaunch), and
:: installed version of each.
FOR %%A IN (
    {8A69D345-D564-463c-AFF1-A69D9E530F96},
    {8237E44A-0054-442C-B6B6-EA0509993955},
    {401C381F-E0DE-4B85-8BD8-3F3F14FBDA57},
    {4ea16ac7-fd5a-47c3-875b-dbf4a2008c20}) DO (
  reg query "HKCU\Software\Google\Update\Clients\%%A" /v name "/reg:32" 2>NUL
  reg query "HKCU\Software\Google\Update\Clients\%%A" /v opv "/reg:32" 2>NUL
  reg query "HKCU\Software\Google\Update\Clients\%%A" /v pv "/reg:32" 2>NUL
)
