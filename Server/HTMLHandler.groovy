
import com.sun.net.httpserver.*

class HTMLHandler implements HttpHandler {

    static boolean answer = false
    
	String filename
	
    void handle( HttpExchange exchange ) {
    
        String requestURI = exchange.requestURI
		filename = requestURI - '/internal/'
		
		if( filename.endsWith( 'html' ) )
			exchange.responseHeaders.set( "Content-Type", "text/html" )
		else if( filename.endsWith( 'css' ) )
			exchange.responseHeaders.set( "Content-Type", "text/css" )

        exchange.responseHeaders.set( "Encoding", "UTF-8" )
        exchange.sendResponseHeaders( 200, 0 )
        exchange.responseBody.withStream { OutputStream stream ->
            doExecute( exchange.requestBody, stream )
        }
        
        println ""
    }

    protected void doExecute( InputStream input, OutputStream output ) {
        println "handle request : " + filename
		
		File f = new File( filename )
		output << f.text
    }
}

