import hashlib

def new(*args):
    return hashlib.new('md4', *args)
