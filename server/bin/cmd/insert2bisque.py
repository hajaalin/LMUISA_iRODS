#!/usr/bin/env python
## insert2bisque.py
## This file lives in IRODSHOME/server/bin/cmd/insert2bisque.py
##       
import sys
import shlex
import urllib
import urllib2
import urlparse
import base64
import logging

############################
# Config for local installation
LOGFILE='/tmp/bisque_insert.log'
BISQUE_HOST='http://lmu-omero1.biocenter.helsinki.fi:8000'
BISQUE_ADMIN_PASS='admin'
IRODS_HOST='irods://lmu-omero1.biocenter.helsinki.fi'
# End Config 

logging.basicConfig(filename=LOGFILE, level=logging.INFO)
log = logging.getLogger('i2b')

def main():
    log.debug( "insert2bisque recevied %s" % (sys.argv) )
    try:
        obj = sys.argv[1]
        user = sys.argv[2]
        url = "%s/import/insert?%s" % (BISQUE_HOST, urllib.urlencode( { 'url': IRODS_HOST+obj, 'user': user}))
        request = urllib2.Request(url)
        request.add_header('authorization',  'Basic ' + base64.encodestring("admin:%s" % BISQUE_ADMIN_PASS ).strip())
        r = urllib2.urlopen(request)
        response = r.read()
        log.info( 'insert %s -> %s' % (url, response))
    except Exception,e:
        log.exception( "exception occurred %s" % e )
        raise e


if __name__ == "__main__":
    main()
    sys.exit(0)
