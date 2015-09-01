#!/usr/bin/env python

"""
Run a multi-threaded single-client USBTMC Server implemented in Python.

Example run:
    ./usbtmc-server.py --backend python_usbtmc USB::0x1ab1::0x04ce::INST


Contains code from https://gist.github.com/pklaus/b741eedc66b5dc01f49a
"""

import argparse, random, logging, sys
from logging import DEBUG, INFO, WARNING, ERROR, CRITICAL

try:
    import socketserver
except ImportError:
    import SocketServer as socketserver
import socket, threading

try:
    from universal_usbtmc import import_backend
    from universal_usbtmc import UsbtmcReadTimeoutError
except ImportError:
    sys.stderr.write('Cannot import the Python package universal_usbtmc, please it with\n`pip install https://github.com/pklaus/universal_usbtmc/archive/master.zip`.\nExiting...\n')
    sys.exit(1)

logger = logging.getLogger('usbtmc-server')

class CmdTCPServer(socketserver.ThreadingTCPServer):
    """
    A TCP server made to respond to line based commands.
    """

    #: newline character(s) to be added to string responses
    newline = '\n'
    #: Ctrl-C will cleanly kill all spawned threads
    daemon_threads = True
    #: much faster rebinding possible
    allow_reuse_address = True
    address_family = socket.AF_INET6

    class CmdRequestHandler(socketserver.StreamRequestHandler):
        def handle(self):
            if not self.server.lock.acquire(blocking=False):
                self.log(DEBUG, 'An additional cliend tried to connect from {client}. Denying...')
                return
            self.log(DEBUG, 'Connected to {client}.')
            try:
                while True:
                    self.single_cmd()
            except Disconnected:
                pass
                self.log(DEBUG, 'The client {client} closed the connection')
            finally:
                self.server.lock.release()
        def read_cmd(self):
            return self.rfile.readline().decode('utf-8').strip()
        def log(self, level, msg, *args, **kwargs):
            if type(level) == str:
                level = getattr(logging, level.upper())
            msg = msg.format(client=self.client_address[0])
            logger.log(level, msg, *args, **kwargs)
        def send_reply(self, reply):
            if type(reply) == str:
                if self.server.newline: reply += self.server.newline
                reply = reply.encode('utf-8')
            self.wfile.write(reply)
        def single_cmd(self):
            cmd = self.read_cmd()
            if not cmd: raise Disconnected
            self.log(DEBUG, 'Received a cmd: {}'.format(cmd))
            try:
                reply = self.server.process(cmd)
                if reply is not None:
                    self.send_reply(reply)
            except:
                self.send_reply('ERR')

    def __init__(self, server_address, name=None):
        socketserver.TCPServer.__init__(self, server_address, self.CmdRequestHandler)
        self.lock = threading.Lock()
        self.name = name if name else "{}:{}".format(*server_address)

    def process(self, cmd):
        """
        Implement this method to handle command processing.
        For each command, this method will be called.
        Return a string or bytes as appropriate.
        If your the message is only a command (not a query), return None.
        """
        raise NotImplemented

class UsbtmcServerExample(CmdTCPServer):

    def process(self, cmd):
        """
        This is the method to process each command
        received from the client.
        """
        if '?' in cmd:
            return self.instr.query(cmd)
        else:
            self.instr.write(cmd)

def main():
    parser = argparse.ArgumentParser(description=__doc__.split('\n\n\n')[0], formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--loglevel', default='INFO', help='log level', choices=['CRITICAL', 'ERROR', 'WARNING', 'INFO', 'DEBUG'])
    parser.add_argument('--port', type=int, default=5025, help='TCP port to listen to.')
    parser.add_argument('--host', default='::', help='The host / IP address to listen at.')
    parser.add_argument('--line-ending', default='', help="The line ending to add to commands you're sending")
    parser.add_argument('--backend', default='linux_kernel', help='The backend to use')
    parser.add_argument('device', help='The device string (format depends on the backend)')
    args = parser.parse_args()

    args.line_ending = args.line_ending.replace("\\n", "\n").replace("\\r","\r")
    try:
        backend = import_backend(args.backend)
        Instrument = backend.Instrument
    except ImportError:
        parser.error('choose a valid backend')

    logging.basicConfig(format='%(message)s', level=args.loglevel.upper())

    scpi_server = UsbtmcServerExample((args.host, args.port))
    instr = Instrument(args.device)
    logger.info('Connected to USBTMC device {}.'.format(instr.idn))
    scpi_server.instr = instr

    try:
        scpi_server.serve_forever()
    except KeyboardInterrupt:
        logger.info('Ctrl-C pressed. Shutting down...')
    scpi_server.server_close()

class Disconnected(Exception): pass

if __name__ == "__main__":
    main()
