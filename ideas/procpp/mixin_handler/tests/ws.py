import sys
sys.path.append('build/python/')

import ReactHandler

class PythonCallback(ReactHandler.WebSocketCallback):
    def run(self, m, ws):
            if m.opcode == ReactHandler.WebSocket.OPCODE_TEXT_FRAME:
                ws.send(m.client, m.payload)

callback = PythonCallback().__disown__()

s = ReactHandler.WebSocketServer()
s.on('message', callback)
s.listen(8082)
s.event_loop()

