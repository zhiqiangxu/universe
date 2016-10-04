import sys
sys.path.append('build/python/')

import ReactHandler

s = ReactHandler.HttpServer()
s.listen(8082)
s.event_loop()

