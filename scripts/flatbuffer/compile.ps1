Get-ChildItem *.fbs | ForEach-Object { bin\flatc.exe --cpp -o out\cpp $_.FullName }
Copy-Item -Path ".\out\cpp\*" -Destination "..\..\servers\common\flatbuffer\" -Force

Get-ChildItem *.fbs | ForEach-Object { bin\flatc.exe --csharp -o out\chsarp $_.FullName }
Get-ChildItem *.fbs | ForEach-Object { bin\flatc.exe --go -o out\go $_.FullName }

pause