#!/usr/bin/env python3
import cgi
import serial
import time
print("""Content-type: text/html

<html>

<head>
<link rel="stylesheet" type="text/css" href="styles/styles.css">
<script src="js/jquery-3.2.1.min.js"></script>
<script src="js/scripts.js"></script>
</head>

<body>

""")

form = cgi.FieldStorage()
cur_user = form.getvalue("cur_user")

headers = ["left_pressure", "up_pressure", "right_pressure", "down_pressure"]
pressures = [0, 0, 0, 0]

# Read all the threshold values.
for i in range(0, 4):
    pressures[i] = form.getvalue(headers[i])

# Opening serial this way because we don't want to reset the board.
s = serial.Serial()
s.port = "COM7"
s.baudrate = 9600
s.setDTR(False)
s.open()

# Read the threshold setting file.
f = open("users.txt", "r")
users_file = f.read()
f.close()

# Build the array of user threshold by splitting "^"
user_list = users_file.split("^")

# Find if we are working with existing user.
cur_user_list_index = -1
for u in range(len(user_list)):
    if user_list[u].split(":")[0] == cur_user:
        cur_user_list_index = u
        break

cur_user_list = user_list[cur_user_list_index].strip("\n").split(":")

valid_pressure_format_flag = True
for i in range(0, 4):
    if (len(pressures[i]) == 3):
        if cur_user_list_index != -1:
            cur_user_list[i+1] = pressures[i]
        s.write((str(i)+str(pressures[i])+"\r\n").encode())
        new_pressures = s.read(78).decode()
    else:
        valid_pressure_format_flag = False

# if they aren't valid pressures, read whatever is in the Arduino.
if (valid_pressure_format_flag is False):
    s.write("7\r\n".encode())
    new_pressures = s.read(78).decode()

print(new_pressures.replace(",", "|"))
print("<br><a href=pads.py?cur_user=%s>Return</a>" % cur_user)

user_list[cur_user_list_index] = ":".join(cur_user_list)
f = open("users.txt", "w")
f.write("^".join(user_list))
f.close

s.close()
print('<script>setTimeout(function() { window.location = "pads.py?cur_user=%s" }, 1000) </script>' % cur_user)

print("</body>")
print("</html>")
