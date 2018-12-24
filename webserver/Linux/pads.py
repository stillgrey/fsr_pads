#!/usr/bin/env python3
import serial
import cgi
print("""Content-type: text/html

<html>

<head>
	<link rel="stylesheet" type="text/css" href="styles/styles.css">
	<script src="js/jquery-3.2.1.min.js"></script>
	<script src="js/scripts.js"></script>
</head>

<body>
	<div id="maincont">
		<div style="display: table-cell; vertical-align: top; width: 120px; height: 590px">
			<iframe src="http://groovestats.com/ECS6/googlesheets/timer.php?machine=Ian" style="width:100px; height: 590px; border-radius: 30px;"></iframe>
		</div>
		<div style="display: table-cell;">
			<div class="mainhead"><img src="img/smallpad.png" style="margin-right: 20px;">1P on ITG-Ian</div>
			<div>
				<div class="hcell" style="border-top: 2px solid #020a12; border-right: 2px solid #020a12; width: 240px; display: table-cell;">
					<form action="/pads.py" method="get">

						<select name="cur_user">""")

f = open("users.txt", "r")
user_list = f.read().split("^")
f.close()

for u in user_list:
    u_name = u.split(":")[0]
    print('<option value="%s">%s</option>' % (u_name, u_name))

print("""</select>
<input type="submit" value="Select User">

	</form>
</div>
<div class="hcell" style="border-top: 2px solid #020a12; width: 358px; display: table-cell;">
	<form action="/user.py" method="get">
		<input type="text" name="new_user"> <input type="submit" value="Create User">
	</form>
</div>
</div>""")

form = cgi.FieldStorage()
cur_user = form.getvalue("cur_user")
s = serial.Serial("/dev/ttyACM0", 9600)
s.setDTR(1)
if (cur_user != "Guest" and cur_user != ""):
    for u in user_list:
        u_array = u.split(":")
        if u_array[0] == cur_user:
            s.write("0"+u_array[1]+"\r\n")
            s.read(78)
            s.write("1"+u_array[2]+"\r\n")
            s.read(78)
            s.write("2"+u_array[3]+"\r\n")
            s.read(78)
            s.write("3"+u_array[4]+"\r\n")
            s.read(78)
            break
s.write("7\r\n")
cur_pressures = s.read(78).split(',')
s.close()


f = open("indexbottom.html", "r")
f.read() % (cur_user, int(cur_pressures[3]), int(cur_pressures[1]), int(cur_pressures[5]), int(cur_pressures[7]))
f.close()
