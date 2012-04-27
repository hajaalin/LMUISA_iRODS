import irods, re
from irods import *
from irods_error import *

def testH0(rei):
    print 'testH0 kukkuu'
    
def testH1(resStr, rei):
    irods.fillStrInMsParam(resStr, 'testH1 kukkuu')

def testH2(inStr, resStr, rei):
    inStr = inStr.parseForStr()
    res = 'testH2 kukkuu ' + str(inStr) + '\n'
    print res
    irods.fillStrInMsParam(resStr, res)

row_map = {'00':'A','01':'B', '02':'C', '03':'D', '04':'E', '05':'F', '06':'G', '07':'H'  }

col_idx_well = 'U'
row_idx_well = 'V'
col_idx_field = 'X'
row_idx_field = 'Y'

# the regexp that defines the well
prog_well = re.compile(col_idx_well + '([0-9]+)' + '--' + row_idx_well + '([0-9]+)')
# the regexp that defines the field
prog_field = re.compile(col_idx_field + '([0-9]+)' + '--' + row_idx_field + '([0-9]+)')
# the regexp that defines the channel
prog_c = re.compile('C([0-9]+).ome.tif')

def findBasename(path):
    dirs = path.split('/')
    base = dirs[len(dirs)-1]
    return base
    
def createPlateCode(path):
    base = findBasename(path)
    plate_code = re.sub('[^0-9]', '', base, 0)
    plate_code = re.sub('20', '', plate_code, 1)
    plate_code = 'LEICA_' + plate_code
    return plate_code

def createWellCode(well):
    result_w = prog_well.search(well)
    well_old = result_w.group(0)
    # make column numbers start from 1
    col = int(result_w.group(1) ) + 1
    row = result_w.group(2)
    well_new = row_map[row] + repr(col).zfill(2)
    return well_new

def findFieldIndexRange(fields):
    xmax = 0
    ymax = 0
    for f in fields:
        #print f
        result_f = prog_field.search(f)
        x = result_f.group(1)
        y = result_f.group(2)
        #print x,  y
        x = int(x)
        y = int(y)
        xmax = max(x, xmax)
        ymax = max(y, ymax)
    # make indices start from 1, not 0    
    return (xmax+1, ymax+1)
    
def createFieldCode(xmax, ymax, field):
    result_f = prog_field.search(field)
    x = result_f.group(1)
    y = result_f.group(2)
    x = int(x) + 1
    y = int(y) + 1
    field_new = (y-1)*xmax + x
    field_new = repr(field_new).zfill(2)
    return field_new

def createChannelCode(image):
    result_c = prog_c.search(image)
    channel = result_c.group(1)
    channel = int(channel)
    return repr(channel)


def msiLeica2Cellomics(inputColl, outputColl, resStr, rei):
    res = ''
    
    inputColl = inputColl.parseForStr()
    outputColl = outputColl.parseForStr()
    print 'msiLeica2Cellomics: ',  inputColl,  outputColl
    
    # irods connection
    conn = rei.getRsComm()
    # get directory handles
    c = irodsCollection(conn)
    res += c.getCollName() + '\n'

    # experiment
    experiment = findBasename(inputColl)
    plateCode = createPlateCode(experiment)
    c.openCollection(outputColl)
    c.createCollection(experiment + '_cellomics')
    outputColl += '/' + experiment + '_cellomics/'
    
    c.openCollection(inputColl)
    res += c.getCollName() + '\n'

    # plate
    c.openCollection('slide--S00')
    
    wells = c.getSubCollections()
    for w in wells:
        # well
        c.openCollection(w)
        res += 'WELL ' + c.getCollName() + '\n'
        wellCode = createWellCode(w)
        
        fields = c.getSubCollections()
        (xmax, ymax) = findFieldIndexRange(fields)
        
        for f in fields:
            c.openCollection(f)
            res += 'FIELD ' + c.getCollName() + '\n'
            fieldCode = createFieldCode(xmax, ymax, f)
            
            for dataObj in c.getObjects():
                image = dataObj[0]
                resc = dataObj[1]
                print image
                channelCode = createChannelCode(image)
                
                cellomicsName = plateCode  + '_'+ wellCode + 'f' + fieldCode+ 'd' + channelCode +'.TIF'
                print cellomicsName
                
                f = c.open(image, 'r', resc)
                f.addUserMetadata('leica.experiment', experiment)
                f.addUserMetadata('cellomics.collection', outputColl)
                f.addUserMetadata('cellomics.plateId', plateCode)
                f.addUserMetadata('cellomics.imageName', cellomicsName)
                f.addUserMetadata('cellomics.dataObject', outputColl + cellomicsName)
                f.close()
                
                res += image + ' '+ cellomicsName + '\n'
            c.upCollection()
        res += '\n'
        c.upCollection()
        
    irods.fillStrInMsParam(resStr, str(res))
    
def msiLeica2FlatDir(inputColl, outputColl, resStr, rei):
    res = ''
    
    inputColl = inputColl.parseForStr()
    outputColl = outputColl.parseForStr()
    print 'msiLeica2FlatDir: ',  inputColl,  outputColl
    
    # irods connection
    conn = rei.getRsComm()
    # get directory handles
    c = irodsCollection(conn)
    res += c.getCollName() + '\n'

    # experiment
    experiment = findBasename(inputColl)
    plateCode = createPlateCode(experiment)
    c.openCollection(outputColl)
    c.createCollection(experiment + '_flatdir')
    outputColl += '/' + experiment + '_flatdir/'
    
    c.openCollection(inputColl)
    res += c.getCollName() + '\n'

    # plate
    c.openCollection('slide--S00')
    
    wells = c.getSubCollections()
    for w in wells:
        # well
        c.openCollection(w)
        res += 'WELL ' + c.getCollName() + '\n'
        wellCode = createWellCode(w)
        
        fields = c.getSubCollections()
        (xmax, ymax) = findFieldIndexRange(fields)
        
        for f in fields:
            c.openCollection(f)
            res += 'FIELD ' + c.getCollName() + '\n'
            fieldCode = createFieldCode(xmax, ymax, f)
            
            for dataObj in c.getObjects():
                image = dataObj[0]
                resc = dataObj[1]
                print image
                
                stackName = plateCode  + '_'+ wellCode + 'f' + fieldCode + '.tif'
                print stackName
                
                f = c.open(image, 'r', resc)
                f.addUserMetadata('leica.experiment', experiment)
                f.addUserMetadata('leica2flatdir.collection', outputColl)
                f.addUserMetadata('leica2flatdir.plateId', plateCode)
                f.addUserMetadata('leica2flatdir.stackName', stackName)
                f.addUserMetadata('leica2flatdir.dataObject', outputColl + stackName)
                f.close()
                
                res += image + ' '+ stackName + '\n'
            c.upCollection()
        res += '\n'
        c.upCollection()
        
    irods.fillStrInMsParam(resStr, str(res))

def msiIsReadyHcsColl0(inStr, resStr, rei):
    inStr = inStr.parseForStr()
    res = 'test msiIsReadyHcsColl0 ' + str(inStr) + '\n'
    print res
    irods.fillStrInMsParam(resStr, res)

def msiIsReadyHcsColl(inStr, resStr, rei):
    res = ''
    coll = inStr.parseForStr()
    print "msiIsReadyHcsColl: " + coll

    # irods connection
    conn = rei.getRsComm()

    # select statement
    i1 = inxIvalPair_t()
    i1.addInxIval(COL_D_CREATE_TIME, 0)
    # sort results
    i1.addInxIval(COL_D_CREATE_TIME, ORDER_BY_DESC)
    
    # condition statement
    i2 = inxValPair_t()
    i2.addInxVal(COL_COLL_NAME, " like '" + coll + "%'")
    
    # create query
    genQueryInp = genQueryInp_t()
    genQueryInp.setSelectInp(i1)
    genQueryInp.setSqlCondInp(i2)
    genQueryInp.setMaxRows(2)
    genQueryInp.setContinueInx(0)
    
    # execute query
    print 'msiIsReadyHcsColl: before query'
    genQueryOut, status = rsGenQuery(conn, genQueryInp)

    if status !=0:
        res += "msiIsReadyLeicaColl: ERROR " + str(status) + '\n'
        
    if status == CAT_NO_ROWS_FOUND:
        res += "msiIsReadyLeicaColl: ERROR " + str(status) + '\n'
       
    # read the times
    for sqlRes in  genQueryOut.getSqlResult():
        for time in sqlRes.getValues():
            res += "time " + str(time) + '\n'
        
    res += "ready tai not\n"
    
    genQueryOut.free()
    print 'msiIsReadyHcsColl: after query'
    res = 'test msiIsReadyHcsColl ' + str(coll) + '\n'
    res = 'READY, query done\n'
    print res
    irods.fillStrInMsParam(resStr, res)
    print 'msiIsReadyHcsColl: exiting'

