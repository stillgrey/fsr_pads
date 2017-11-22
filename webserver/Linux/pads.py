#!/usr/bin/env python
import serial
import cgi
print "Content-type: text/html"
print
print '''<form action="/user.py" method="get">'''
print '''New User: <input type="text" name="new_user">'''
print '''<input type="submit" value="Create New User">'''
print '''</form>'''
print '''<form action="/pads.py" method="get">'''
f = open("users.txt", "rb")
user_list = f.read().split("^")
f.close()
print '''<select name="cur_user">'''
for u in user_list:
    u_name = u.split(":")[0]
    print '''<option value="%s">%s</option>''' % (u_name, u_name)

print ''' </select>'''
print ''' <input type="submit" value="Select User"> '''
print '''</form>'''
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
f = open("index.html", "rb")
print f.read() % (cur_user, int(cur_pressures[1]), int(cur_pressures[3]), int(cur_pressures[5]), int(cur_pressures[7]))
f.close()
