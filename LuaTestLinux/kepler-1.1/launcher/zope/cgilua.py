import lua
import urllib
import Acquisition
import string
import ZPublisher
import OFS.PropertyManager
from threading import Semaphore
from Products.PageTemplates.ZopePageTemplate import ZopePageTemplate
from Globals import DTMLFile
from Globals import Persistent
from OFS import SimpleItem
from OFS.History import Historical

# Edit the following line to point to where you installed the cgilua launcher
product_folder = 'd:/Zope-Instance/Products/CGILua'

lua_addForm=DTMLFile('dtml/add',globals())
lua_mutex = Semaphore()

def lua_add(self, id, title='', docroot='', REQUEST=None):
    """test of lua
    """
    id = str(id)
    l = CGILua(id, title, docroot)
    self._setObject(l.getId(), l)
    if REQUEST is not None:
        REQUEST['RESPONSE'].redirect(self.absolute_url()+'/manage_main')

class CGILua(Acquisition.Implicit, Persistent, SimpleItem.Item_w__name__, OFS.PropertyManager.PropertyManager, ZopePageTemplate):
  "A Lua Product"
  meta_type='lua cgi file'
  manage_options    = (
      OFS.PropertyManager.PropertyManager.manage_options
      +OFS.SimpleItem.SimpleItem.manage_options
  )
  __ac_permissions__ = (
        (
            ('Access contents information',
                ('id', 'title', 'title_or_id', 'title_and_id', ),
                ('Anonymous','Manager', ),
            ),
            ('View',
                ('__repr__', '', ),
                ('Anonymous', 'Manager', ),
            ),
            ('Manage CGILua proxies',
                ( ),
                ('Manager', ),
            ),
        )
  )
  _properties=(
      {'id' : 'title', 'type': 'string', 'mode': 'w'},
      {'id' : 'docroot', 'type' : 'string', 'mode': 'w'},
  )
  def __init__(self, id, title, docroot):
     "Initialize an instance"
     self.__name__ = id
     self.title = title
     self.docroot = docroot
  def index_html(self):
     """ run CGI-BIN program. """
     self.REQUEST.stdin.seek(0)
     self.post_data = self.REQUEST.stdin.read()
     self.post_bytes = len(self.post_data)
     
     env = {}
     for k in self.REQUEST.environ.keys():
       if '.' not in k: #to avoid copying 'channel.creation_time'
         env[k] = self.REQUEST.environ[k]
     if self.REQUEST['QUERY_STRING']:
       env['REQUEST_URI'] = env['SCRIPT_NAME'] + env['PATH_INFO'] + "?" + str(self.REQUEST['QUERY_STRING'])
     else:
       env['QUERY_STRING'] = ''
       env['REQUEST_URI'] = env['SCRIPT_NAME'] + env['PATH_INFO']
     env['PATH_TRANSLATED'] = self.docroot + env['PATH_INFO']
     env['SCRIPT_NAME'] = env['PATH_INFO']
     env['DOCUMENT_ROOT'] = self.docroot
     env['SERVER_ADDR'] = string.split(env['HTTP_HOST'], ':')[0]
     if string.upper(env['REQUEST_METHOD']) == 'POST':
       env['CONTENT_LENGTH'] = str(self.post_bytes)
     else:
       env['CONTENT_LENGTH'] = ''
     self.server_vars = env
     lua_mutex.acquire()
     lua.globals().zope = self
     self.html = ''
     lua.execute("dofile('" + product_folder + "/zope.lua')")
     lua_mutex.release()
     return self.html
  def lua_add(self, RESPONSE):
     self._setObject('Lua_id', lua('lua_id'))
     RESPONSE.redirect('index_html')
  def id(self):
      return self.__name__
  def title_or_id(self):
      """ """
      if self.title:
          return self.title
      else:
          return self.getId()
  def title_and_id(self):
      """ """
      if self.title:
          return "%s (%s)" % (self.title, self.getId())
      else:
          return "%s" % (self.getId(),)
  def lua_edit(self, REQUEST, RESPONSE):
      "Edits object data"
      self.title = REQUEST['title']
      self.docroot = REQUEST['docroot']
      return RESPONSE.redirect('index_html')
  def redirect(self, url):
      self.REQUEST.RESPONSE.redirect(url)
  def set_status(self, status):
      self.REQUEST.RESPONSE.setStatus(status)
  def set_header(self, header, value):
      self.REQUEST.RESPONSE.setHeader(header, value)
  def write(self, s):
      self.html = self.html + s
  def log_error(self, s):
      print s
  def get_total_bytes(self):
      return self.post_bytes
  def get_post_data(self):
      return self.post_data
  def get_server_variable(self, name):
      return self.server_vars[name]
  def _bt_processing(self, TraversalRequest, name):
      """ Called by __bobo_traverse__ (In the 'Magic' section) - ZCGIScript Version """
      # If there's no more names left to handle, return __repr__
      # to the traversal machinery so it gets called next
      if len(TraversalRequest['TraversalRequestNameStack'])==0:
          if TraversalRequest.environ['PATH_INFO'][-1] == '/':
              TraversalRequest['PATH_STACK'].append('')
          TraversalRequest.environ['PATH_INFO'] = string.join([''] + TraversalRequest['PATH_STACK'], '/')
          TraversalRequest.environ['SCRIPT_NAME'] = string.replace(self.absolute_url(), TraversalRequest['BASE0'], '')
          TraversalRequest.environ['REQUEST_URI'] = string.replace(TraversalRequest['URL0'], TraversalRequest['BASE0'], '')
          return getattr(self, 'index_html')
      # If there are more names to handle, return ourself, so our __bobo_traverse__ gets
      # called again and we get to ignore the other names.
      return self
  def __bobo_traverse__(self, TraversalRequest, name):
      """ Used in handling PATH_INFO, that is filepath information after the script name """
      if not TraversalRequest.has_key('PATH_STACK'):
          TraversalRequest['PATH_STACK'] = []
          TraversalRequest['REMOVE_STACK'] = []
      # If name is a method/attribute of this object, call it
      TraversalRequest['REMOVE_STACK'].append(name)
      if name!='index_html':
          TraversalRequest['PATH_STACK'].append(name)
          if hasattr(self.aq_base, name):
              return getattr(self, name)
        # otherwise do some 'magic'
      return self._bt_processing(TraversalRequest, name)
#      """ Used in handling PATH_INFO, that is filepath information after the script name """
#      print(name)
#      self.REQUEST.environ["SCRIPT_NAME"] = name
#      # otherwise do some 'magic'
#      return self.index_html

import Globals
Globals.InitializeClass(CGILua)
