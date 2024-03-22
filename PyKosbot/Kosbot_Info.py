buffer_size = 2048
version = "1.0"
serv_host = "127.0.0.1"
serv_port = 8000

def wrap_request(to_wrap):
    cmd = {
        "Kosbot" : version,
        "command" : to_wrap
    }
    return cmd

def request_connection():
    cmd = {
        "request" : "request_connection"
    }
    
    return wrap_request(cmd)

def disconnect_from_kostbot(connection_id: int):
    cmd = {
        "request" : "disconnect",
        "connection_id" : connection_id
    }
    
    return wrap_request(cmd)