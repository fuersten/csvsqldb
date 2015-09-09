-- configuration comment

server = {}

server.threads = 100
server.port = 10474
server.api = {}
server.api.code = 'LUA'

port = 10474
factor = 47.11
hostname = 'Thor'

daemonize = true

test = {}
test.wtf = 815

debug.global_level = 1

debug.level = { tcp_server = 2, connection = 3, filesystem = 1 }
-- debug.level.tcp_server = 2
-- debug.level.connection = 3
-- debug.level.filesystem = 1


function testparams(text, i, n)
    return 'This is a test: ' .. text
end

function dosomething()
    doit(2,7.5)
    done()
    print(mul(2,15))
    print('hi ho')
end

