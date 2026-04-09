import traceback

from .error import *

def report_init_error(func):
    def inner(self, obj: dict, *args, **kwargs):
        try:
            func(self, obj, *args, **kwargs)
        except SyzlangObjInitError as e:
            raise e
        except KeyError as e:
            traceback.print_exc()
            msg = "{} is not defined"
            raise SyzlangObjInitError(obj, self.TYPE, msg)
        except Exception as e:
            traceback.print_exc()
            raise SyzlangObjInitError(obj, self.TYPE, e)
    return inner 

def report_str_error(func):
    def inner(self):
        try:
            return func(self)
        except SyzlangObjInitError as e:
            raise SyzlangObjStrError(self, self.TYPE, e)
        except SyzlangObjStrError as e:
            raise e
        except Exception as e:
            traceback.print_exc()
            raise SyzlangObjStrError(self, self.TYPE, e)
    return inner