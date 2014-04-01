![Civetta](http://i.imgur.com/ewylnEI.jpg "Civetta") Civetta
=======

Civetta is a C++11 layer above the great [https://github.com/bel2125/civetweb](https://github.com/bel2125/civetweb "civetweb"). It is also inspired by [https://github.com/Gregwar/mongoose-cpp](https://github.com/Gregwar/mongoose-cpp "mongoose-cpp"). Please refer to Civetweb for more information. License is [MIT](http://en.wikipedia.org/wiki/MIT_License).
	
Usage
-----

	#include <civetta\Server.h>
	#include <iostream>
	
	int main(int argc, char *argv[]) {
	
	  const char *options[] = { "document_root", ".", "listening_ports", "8081",
	                            0 };
	
	  Civetta::Server server(options);
	  server.route("GET", "/civetta", [](Civetta::Request &req, Civetta::Response &res) {
			res << "Hello World!";
		});
	
	  std::cout << "Press <ENTER> to exit" << std::endl;
	  std::cin.ignore();
	  return 0;
	}



Support
-------

The original Mongoose project is recommended if support is needed.  However,
this is very easy to install and use. 

Contributions
---------------

Contributions are welcome provided all contributions carry the MIT license.

DO NOT APPLY fixes copied from Mongoose to this project to prevent GPL tainting.

### Author

The original Author was Sergey Lyubka.  He still controls the original
Mongoose project.  However, he has changed or said he would 
change licenses after writing and distributing the original code this
project is based on.

Using this project ensures the MIT licenses terms are applied and
GPL cannot be imposed on any of this code as long as it is sourced from
here.

For the latest and greatest code and features, please use the Mongoose
code from Sergey Lyubka.  However, doing so will require adherence to
the new licenses.

This project ensures that GPL license cannot be applied to this code.
This code will remain free with the MIT license protection.
