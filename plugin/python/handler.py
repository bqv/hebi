
class Handler(object):
    def __init__(self, sendfn):
        self.sendfn = sendfn

    def handle(line):
        print("In Python")
        self.send("PRIVMSG ##doge :python")

    def send(line):
        self.sendfn(line)
