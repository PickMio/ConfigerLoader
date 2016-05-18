rand: LoadConfig.cpp
	g++ LoadConfig.cpp  -o rand ./pugixml/pugixml.a
clean:
	rm rand
run:
	./rand
    
