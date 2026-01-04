Get-ChildItem *.fbs | ForEach-Object { bin\flatc.exe --cpp -o out\cpp $_.FullName }
Get-ChildItem *.fbs | ForEach-Object { bin\flatc.exe --csharp -o out\chsarp $_.FullName }
Get-ChildItem *.fbs | ForEach-Object { bin\flatc.exe --go -o out\go $_.FullName }