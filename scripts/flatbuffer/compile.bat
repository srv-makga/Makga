bin\flatc.exe --scoped-enums --cpp -o out\cpp enum.fbs
bin\flatc.exe --scoped-enums --cpp -o out\cpp result.fbs
bin\flatc.exe --scoped-enums --cpp -o out\cpp version.fbs
bin\flatc.exe --scoped-enums --cpp -o out\cpp game_packet_id.fbs
bin\flatc.exe --scoped-enums --cpp -I . -o out\cpp game_packet.fbs
bin\flatc.exe --scoped-enums --cpp -o out\cpp login_packet_id.fbs
bin\flatc.exe --scoped-enums --cpp -I . -o out\cpp login_packet.fbs