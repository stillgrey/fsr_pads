#!/usr/bin/env python3
import http.server
import cgitb
cgitb.enable()  ## This line enables CGI errorreporting

server = http.server.HTTPServer
handler = http.server.CGIHTTPRequestHandler
server_address = ("", 80)
handler.cgi_directories = ["/", "/cgi-bin"]

httpd = server(server_address, handler)
httpd.serve_forever()
