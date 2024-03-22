import numpy as np
from threading import Thread
import socket
import json
import Kosbot_Server
import Kosbot_Info

def connect_to_kosbot(sock: socket.socket):
    req = {
        "request" : "request_connection"
    }
    msg = Kosbot_Info.wrap_request(req)
    
    js_req = json.dumps(msg)
    bt_js = str.encode(js_req)
    
    sock.sendall(bt_js)
    data = sock.recv(1024)
    
    js = json.loads(data)
    
    response = js["command"]
    con_id = response["connection_id"]
    return con_id

def send_to_kosbot_server(socket: socket.socket, req: json):
    js_req = json.dumps(req)
    bt_js = str.encode(js_req)
    
    socket.sendall(bt_js)
    data = sock.recv(1024)

def disconnect_from_kosbot(socket: socket.socket, con_id: int):
    msg = Kosbot_Info.disconnect_from_kostbot(con_id)
    js_req = json.dumps(msg)
    bt_js = str.encode(js_req)
    sock.sendall(bt_js)
    return

def send_close_request(socket: socket.socket):
    cmd = {
        "request" : "close_kosbot",
        "connection_id" : 0
    }
    msg = Kosbot_Info.wrap_request(cmd)
    js_req = json.dumps(msg)
    bt_js = str.encode(js_req)
    sock.sendall(bt_js)
    
    
if __name__ == "__main__":
    server = Thread(target=Kosbot_Server.run_serv)
    server.start()
    
    HOST = Kosbot_Info.serv_host
    PORT = Kosbot_Info.serv_port
    
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((HOST, PORT))
 
    con_id = connect_to_kosbot(sock)
    print(f"Connection_ID : {con_id}")
    
    running = True
    while running:
        print("0. Kosbot Info")
        print("1. Send Robot Command")
        print("2. Disconnect and close.")
        
        ans = int(input("[0-1] : "))
        
        if ans == 0:
            cmd = {
                "request" : "print_kosbot_info",
                "connection_id" : con_id
            }
            req = Kosbot_Info.wrap_request(cmd)
            send_to_kosbot_server(sock, req)
            
        elif ans == 1:
            print("Retry...")
            
        elif ans == 2:
            disconnect_from_kosbot(sock, con_id)
            send_close_request(sock)
            sock.close()
            break
        else:
            print("Not recognized...")
            continue
    
    server.join()
    
    exit()
    
    