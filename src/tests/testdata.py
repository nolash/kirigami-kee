import lmdb
import tempfile
import logging
import os
import shutil

logging.basicConfig(level=logging.DEBUG)
logg = logging.getLogger()

d = os.path.dirname(__file__)
d = os.path.join(d, 'testdata_mdb')
logg.info('using ' + d)

try:
    shutil.rmtree(d)
except FileNotFoundError:
    pass
os.makedirs(d)
env = lmdb.open(d)
dbi = env.open_db()

pfx = b'\x00\x00\x00'
pfx_two = b'\x00\x00\x01'

vals = [
    "foo",
    "bar",
    "baz"
        ]

with env.begin(write=True) as tx:
    for i in range(len(vals)):
        v = i.to_bytes(2, byteorder='big')
        tx.put(pfx + v, vals[i].encode('utf-8'))
    tx.put(pfx_two + b'\x00\x00', b'xyzzy')
    tx.put(pfx_two + b'\x00\x01', b'plugh')

with env.begin() as tx:
    c = tx.cursor()
    if not c.set_range(b'\x00\x00\x01'):
        raise ValueError("no key")
    for k, v in c:
        logg.debug('have {} {}'.format(k, v))
