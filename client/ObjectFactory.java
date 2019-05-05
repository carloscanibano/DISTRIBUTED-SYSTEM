
package client;

import javax.xml.bind.JAXBElement;
import javax.xml.bind.annotation.XmlElementDecl;
import javax.xml.bind.annotation.XmlRegistry;
import javax.xml.namespace.QName;


/**
 * This object contains factory methods for each 
 * Java content interface and Java element interface 
 * generated in the client package. 
 * <p>An ObjectFactory allows you to programatically 
 * construct new instances of the Java representation 
 * for XML content. The Java representation of XML 
 * content can consist of schema derived interfaces 
 * and classes representing the binding of schema 
 * type definitions, element declarations and model 
 * groups.  Factory methods for each of these are 
 * provided in this class.
 * 
 */
@XmlRegistry
public class ObjectFactory {

    private final static QName _Format_QNAME = new QName("http://format/", "format");
    private final static QName _FormatResponse_QNAME = new QName("http://format/", "formatResponse");

    /**
     * Create a new ObjectFactory that can be used to create new instances of schema derived classes for package: client
     * 
     */
    public ObjectFactory() {
    }

    /**
     * Create an instance of {@link Format }
     * 
     */
    public Format createFormat() {
        return new Format();
    }

    /**
     * Create an instance of {@link FormatResponse }
     * 
     */
    public FormatResponse createFormatResponse() {
        return new FormatResponse();
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link Format }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://format/", name = "format")
    public JAXBElement<Format> createFormat(Format value) {
        return new JAXBElement<Format>(_Format_QNAME, Format.class, null, value);
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link FormatResponse }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://format/", name = "formatResponse")
    public JAXBElement<FormatResponse> createFormatResponse(FormatResponse value) {
        return new JAXBElement<FormatResponse>(_FormatResponse_QNAME, FormatResponse.class, null, value);
    }

}
