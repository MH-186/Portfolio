@echo off
Title Network status
chcp 65001 >nul
mode con: cols=32 lines=15
:loop
for /f "tokens=2 delims=:" %%a in ('netsh wlan show interface ^| find "SSID" ^| findstr /v "BSSID"') do set ssid=%%a
for /f "tokens=2 delims=:" %%a in ('netsh wlan show interface ^| find "State" ^| findstr /v "BSSID"') do set state=%%a
for /f "tokens=2 delims=:" %%a in ('netsh wlan show interface ^| find "Signal" ^| findstr /v "BSSID"') do set signal=%%a
for /f "tokens=4 delims==" %%a in ('ping -n 2 8.8.8.8 ^| find "Average"') do set ping=%%a
cls
echo. 
echo       ╔═╗╔╦╗╔═╗╔╦╗╦ ╦╔═╗
echo       ╚═╗ ║ ╠═╣ ║ ║ ║╚═╗
echo       ╚═╝ ╩ ╩ ╩ ╩ ╚═╝╚═╝   
echo       Name: %ssid%                                                                                                     
echo      ╠═══════════════════╣ 
echo        State: %state% 
echo      ╠═══════════════════╣ 
echo        Signal: %signal%    
echo      ╠═══════════════════╣  
echo        Ping: %ping%       
echo      ╠═══════════════════╣ 
goto loop

