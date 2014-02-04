#!/usr/bin/env groovy
// derived from here:
// https://coderwall.com/p/culgjq

import com.sun.net.httpserver.*

String setHTMLHandler( HttpServer server ) {
    String path = "/internal"
    HTMLHandler h = new HTMLHandler()
    HttpContext dlContext = server.createContext( path, h as HttpHandler )
    return path
}

def main() {
    System.setOut( new PrintStream( new FileOutputStream( FileDescriptor.out ), true, "CP850" ) )
    int port = 8890
    HttpServer server = HttpServer.create( new InetSocketAddress( port ), 0 )
    String pathHTML = setHTMLHandler( server )

    println ""
    println "Running on port ${port}..."
    println "HTML   : \"http://127.0.0.1:${port}${pathHTML}\""
    println ""

    server.start()
}

main()



