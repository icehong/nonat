# coding=utf-8
# TCP服务器端程序
import socket
import time
import threading
import cmd


def proc_user(sock):
    sock.send('331 login ok\r\n'.encode())
def proc_pass(sock):
    sock.send('230 access granted\r\n'.encode())
def proc_cmd(sock):
    sock.send('250 CWD\r\n'.encode())
def proc_port(sock):
    sock.send('200 port got.\r\n'.encode())
      

class Cmdproc:
    def __init__(self,cmd,proc):
        self.cmd = cmd
        self.proc = proc

cmd_list = [Cmdproc('USER ', proc_user),
            Cmdproc('PASS ', proc_pass),
            Cmdproc('CWD ', proc_cmd ),
            Cmdproc('PORT ', proc_port)
            ]

def connect_client(ip, port):
    address = (ip, port)
    new = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    print(address)
    new.connect(address)
    new.send('test.'.encode())
    new.close() 

def tcplink(sock, addr):
    print("accept new connection from %s:%s..." % addr)
    sock.send("220 Fake FTP Server\r\n".encode())
    while True:
        data = sock.recv(1024)
        time.sleep(1)
        if not data:
            break
        data_str = data.decode('utf8')
        for i in range(len(cmd_list)):
            if cmd_list[i].cmd in data_str:
                cmd_list[i].proc(sock)
                break
        
        if 'PORT ' in data_str:
            break;      
        
    port_str = data_str
    print(port_str)
    port_str = port_str[5:]
    ip_port = port_str.split(',')
    
    str_ip = '{0}.{1}.{2}.{3}'.format(ip_port[0], ip_port[1], ip_port[2], ip_port[3])
    int_port = int(ip_port[4])*256+ int(ip_port[5])
    str_ip_port = '200 ' + str_ip + '\t' + str(int_port) + '\r\n'
    print(str_ip_port)
    sock.send(str.encode(str_ip_port))
    
    #connect_client(str_ip,int_port)
    time.sleep(300)
    sock.close()
    print("Connection from %s:%s closed." % addr)


s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind(('0.0.0.0', 21))  # 监听端口

s.listen(5)
print("Waiting for connection......")

while True:
    sock, addr = s.accept()
    t = threading.Thread(target=tcplink, args=(sock, addr))
    t.daemon = True
    t.start()
