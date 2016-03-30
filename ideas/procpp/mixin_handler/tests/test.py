import sys
sys.path.append('build/python/')

import ReactHandler

class PythonCallback(ReactHandler.HttpRequestCallback):
    def run(self, req, resp):
            resp.body = 'content from python'

callback = PythonCallback().__disown__()

s = ReactHandler.HttpServer()
s.on('request', callback)
s.listen(8082)
s.start()

