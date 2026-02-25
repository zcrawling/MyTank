from arduino.app_utils import *
import time


def loop():
  Bridge.call("loops", )
  time.sleep(100)


App.run(loop)
