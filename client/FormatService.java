
package client;

import javax.jws.WebMethod;
import javax.jws.WebParam;
import javax.jws.WebResult;
import javax.jws.WebService;
import javax.xml.bind.annotation.XmlSeeAlso;
import javax.xml.ws.Action;
import javax.xml.ws.RequestWrapper;
import javax.xml.ws.ResponseWrapper;


/**
 * This class was generated by the JAX-WS RI.
 * JAX-WS RI 2.2.9-b130926.1035
 * Generated source version: 2.2
 * 
 */
@WebService(name = "FormatService", targetNamespace = "http://format/")
@XmlSeeAlso({
    ObjectFactory.class
})
public interface FormatService {


    /**
     * 
     * @param arg0
     * @return
     *     returns java.lang.String
     */
    @WebMethod
    @WebResult(targetNamespace = "")
    @RequestWrapper(localName = "format", targetNamespace = "http://format/", className = "client.Format")
    @ResponseWrapper(localName = "formatResponse", targetNamespace = "http://format/", className = "client.FormatResponse")
    @Action(input = "http://format/FormatService/formatRequest", output = "http://format/FormatService/formatResponse")
    public String format(
        @WebParam(name = "arg0", targetNamespace = "")
        String arg0);

}
