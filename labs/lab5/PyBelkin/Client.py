from Messages import *
import threading
from copy import deepcopy
from typing import Callable


class ClientBelkin:
    m = MessageBelkin()
    msgs = []
    name = ""
    isConnected = False
    event = threading.Event()
    mutex = threading.Lock()
    clients = []

    @classmethod
    def worker(cls):
        try:
            cls.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            cls.client_socket.connect(("127.0.0.1", 12345))

            MessageBelkin(Type=MessageTypesBelkin.MT_CONNECT).send(cls.client_socket)
        except Exception:
            cls.event.set()
            return

        cls.isConnected = cls.m.receive(cls.client_socket)

        if cls.isConnected:
            cls.name = cls.m.Data
            cls.event.set()

        while cls.isConnected:
            cls.isConnected = cls.m.receive(cls.client_socket)

            if not cls.isConnected:
                cls.name = ""
                break

            match cls.m.Header.Type:
                case MessageTypesBelkin.MT_DATA:
                    with cls.mutex:
                        cls.msgs.append(deepcopy(cls.m))
                    

                case MessageTypesBelkin.MT_INFO:
                    cls.clients = list(map(int, cls.m.Data.split(',')[:-1]))
                    cls.event.set()

        cls.client_socket.close()
        cls.event.set()
        return

    @classmethod
    def run(cls):
        while True:
            print()
            print("---Меню---")
            if cls.isConnected:
                print("подключен")
                print(f"клиент: {cls.name}")
            else:
                print("не подключен")
            print("0 - выход")
            print("1 - обновить")
            print("2 - подключиться") if not cls.isConnected else print("2 - отключиться")
            print("3 - посмотреть сообщения")
            if cls.isConnected:
                print("4 - отправить сообщение")

            choice = ClientBelkin.get_choice(lambda x: (x not in [0, 1, 2, 3, 4]) or (x == 4 and not cls.isConnected))

            match choice:
                case 0:
                    if cls.isConnected:
                        MessageBelkin(Type=MessageTypesBelkin.MT_DISCONNECT).send(cls.client_socket)
                        cls.event.wait()
                        cls.event.clear()
                    print("до свидания!")
                    return

                case 1:
                    continue

                case 2:
                    if cls.isConnected:
                        MessageBelkin(Type=MessageTypesBelkin.MT_DISCONNECT).send(cls.client_socket)
                    else:
                        threading.Thread(target=cls.worker).start()
                    cls.event.wait()
                    cls.event.clear()

                case 3:
                    cls.show_msgs()

                case 4:
                    cls.send_msg()

    @classmethod
    def show_msgs(cls):
        while True:
            print()
            print("---Сообщения---")
            if len(cls.msgs) == 0:
                print("нет сообщений")
            else:
                with cls.mutex:
                    print(*cls.msgs, sep="\n")

            print("0 - назад")
            print("1 - обновить")

            choice = ClientBelkin.get_choice(lambda x: x not in [0, 1])

            if not choice:
                break

    @classmethod
    def send_msg(cls):
        while True:
            print()
            print("---отправка---")
            if cls.isConnected:
                MessageBelkin(Type=MessageTypesBelkin.MT_INFO).send(cls.client_socket)
            else:
                return

            cls.event.wait()
            cls.event.clear()

            print(f"доступные адресаты: {cls.clients}")

            print("0 - назад")
            if len(cls.clients) > 0:
                print("1 - ввести сообщение")

            choice = ClientBelkin.get_choice(lambda x: x not in [0, 1])

            if choice == 0:
                return
            
            text = ""
            ss = input("введите сообщение(.0 - конец ввода): ")
            while ss != ".0":
                text += ss + "\r\n"
                ss = input()

            text = text.rstrip()
            if text == "":
                continue

            flag = False
            while not flag:
                print("0 - отмена")
                print("1 - отправить всем")
                print("2 - отправить одному")

                choice = ClientBelkin.get_choice(lambda x: x not in [0, 1, 2])

                match choice:
                    case 0:
                        break

                    case 1 if cls.isConnected:
                       MessageBelkin(Type=MessageTypesBelkin.MT_DATA, Data=text, To=-1).send(cls.client_socket)
                       flag = True

                    case 2:
                        print(f"доступные адресаты: {cls.clients}")
                        print(f"введите адресата(-1 - отмена)")

                        choice = ClientBelkin.get_choice(lambda x: x not in cls.clients + [-1])

                        if choice == -1:
                            continue
    
                        if cls.isConnected:
                            MessageBelkin(Type=MessageTypesBelkin.MT_DATA, Data=text, To=choice).send(cls.client_socket)
                            flag = True
            else:
                if cls.isConnected:
                    cls.event.wait()
                    cls.event.clear()

    @staticmethod
    def get_choice(non_valid_func: Callable[[int], bool]) -> int:
        while True:
            try:
                choice = int(input())
                if non_valid_func(choice):
                    raise ValueError
                return choice
            except ValueError:
                print("введите правильное число")
                continue
