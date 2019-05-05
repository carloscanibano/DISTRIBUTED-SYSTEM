package format;

import javax.jws.WebService;
import javax.jws.WebMethod;
import java.util.*;
import com.dataaccess.webservicesserver.*;
import java.math.BigInteger;

@WebService
	public class FormatService {
		@WebMethod
			public String format(String msg) {
				//Establecemos comunicacion con el servicio web de conversion de numeros
				NumberConversion service = new NumberConversion();
				NumberConversionSoapType port =  service.getNumberConversionSoap();
				//Eliminamos los espacios multiples que encontremos
				msg = msg.replaceAll("\\s+", " ");
				//Separamos los fragmentos de texto del mensaje original
				String[] w = msg.split("[0-9]+");
				ArrayList<String> words = new ArrayList<String>(Arrays.asList(w));
				//Eliminamos trozos vacios de la lista
				words.removeAll(Arrays.asList(" ", null));
				//Separamos los numeros del texto original
				String[] ncn = msg.split("([a-z]|[A-Z]|\\s)+");
				ArrayList<String> nonConvertedNumbers = new ArrayList<String>(Arrays.asList(ncn));
				//Eliminamos trozos vacios de la lista
				nonConvertedNumbers.removeAll(Arrays.asList("", null));
				//Aqui almacenaremos los numeros ya convertidos
				String[] convertedNumbers = new String[nonConvertedNumbers.size()];
				//Mensaje de respuesta final
				String answer = "";
				//Fragmento formateado con el numero transformado
				String chunk = "";
				//No tiene por que ser igual la cantidad de numeros que de trozos de texto
				int counter = 0;

				for (String number : nonConvertedNumbers) {
					//Insertamos fragmento de texto si no estamos fuera de limites
					if (counter < words.size()) answer = answer.concat(words.get(counter));
					//Convertimos el numero
					convertedNumbers[counter] = port.numberToWords(new BigInteger(number.trim()));
					//Damos formato correcto al numero convertido
					chunk = "(" + convertedNumbers[counter].trim() + ")";
					if (counter < words.size()) chunk = chunk.concat(" ");
					answer = answer.concat(number + " ");
					answer = answer.concat(chunk);
					chunk = "";
					counter++;
				}
				//Si todavia no hemos terminado de concatenar los fragmentos de texto...
				if (counter < words.size()) {
					do {
						answer = answer.concat(words.get(counter));
						counter++;
					} while (counter < words.size());
				}
				//Si no hay numeros en el mensaje, solo quitamos los espacios problematicos
				if (nonConvertedNumbers.isEmpty()) answer = msg;

				return answer.replaceAll("\\s+", " ");
			}
	}