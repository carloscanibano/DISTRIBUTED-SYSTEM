package format;

import javax.xml.ws.Endpoint;
import gnu.getopt.Getopt;

public class FormatServicePublisher {
	public static void main(String[] args) {
		//Modify host and port if you want others...
		final String url = "http://localhost:2000/rs";
		System.out.println("Publishing FormatService at endpoint " + url);
		Endpoint.publish(url, new FormatService());
	}
}