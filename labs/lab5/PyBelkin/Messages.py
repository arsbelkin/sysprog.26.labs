from dataclasses import dataclass
import socket, struct


@dataclass
class MessageTypesBelkin:
    MT_CONNECT: int = 0
    MT_DISCONNECT: int = 1
    MT_INFO: int = 2
    MT_DATA: int = 3


@dataclass
class MessageHeaderBelkin:
    Type: int = MessageTypesBelkin.MT_INFO
    Size: int = 0
    To: int = -2
    From: int = 0


class MessageBelkin:
    def __init__(self, To=-2, From=0, Type=MessageTypesBelkin.MT_INFO, Data=""):
        self.Header = MessageHeaderBelkin(Type, len(Data) * 2, To, From)
        self.Data = Data

    def __str__(self):
        return f"[{self.Header.From}]: {self.Data}"

    def send(self, s: socket.socket):
        s.send(struct.pack(f"iiii", self.Header.Type, self.Header.Size, self.Header.To, self.Header.From))

        if self.Header.Size > 0:
            s.send(struct.pack(f'{self.Header.Size}s', self.Data.encode('utf-16-le')))

    def receive(self, s: socket.socket) -> bool:
        try:
            (self.Header.Type, self.Header.Size, self.Header.To, self.Header.From) = struct.unpack('iiii', s.recv(16))

            if self.Header.Size > 0:
                self.Data = struct.unpack(f'{self.Header.Size}s', s.recv(self.Header.Size))[0].decode('utf-16-le')

            return True
        except Exception:
            return False
