import zmq
import json
import base64
import subprocess
import time
import os

class a_POST_handler:
  def __init__(self,a_number):
    self.number=a_number

  def post(self,request_handler):
	# check request headers
        if not request_handler.headers.has_key('content-length'):
          request_handler.send_error(550,"No content-length given")
        try:
          content_length = int(request_handler.headers['content-length'])
        except ValueError:
          content_length = 0
        if content_length<=0:
          request_handler.send_error(551,"invalid content-length given")

        # if text is given, response appropriately
        if request_handler.headers.has_key('content-type') and request_handler.headers['content-type']=='text':
          text = request_handler.rfile.read(content_length)

          # retrieve the result from zmq and return
          resp = self.genSegmentation(text)
          # convert to JSON
          resp = self.convertToJSON(resp)

          self.sendResponse(resp,request_handler)
        else:
          request_handler.send_error(552, "No or unrecognized content-type")  

  def sendResponse(self,body,request_handler):
        request_handler.send_response( 200 )
        request_handler.send_header( "content-type", "text" )
        request_handler.send_header( "content-length", str(len(body)) )
        request_handler.end_headers()
        request_handler.wfile.write( body )
  
  def genSegmentation(self, text):
        down_image_loc = '/tmp/segtemp/qimg.jpg'
        # download and save the image
        down_cmd = 'wget --no-check-certificate ' + text + ' --output-document="' + down_image_loc + '"'
        subprocess.call(down_cmd, shell=True)
        context = zmq.Context()
        socket = context.socket(zmq.REQ)
        socket.connect("tcp://localhost:5556")
        # strings must be null-delimited (C style)
        socket.send(down_image_loc)
        socket.recv()
        result_fpath = '/srv2/rgirdhar/Work/Code/0005_ObjSegment/scripts/service_scripts/temp-dir/result.jpg'
        while not os.path.exists(result_fpath) and num_tries > 10:
          time.sleep(1)
        f = open(result_fpath, 'rb')
        seg = f.read()
        seg_encoded = base64.b64encode(seg)
        return seg_encoded
  
  def convertToJSON(self, resp):
        output = [('seg', resp)]
        return json.dumps(output)
