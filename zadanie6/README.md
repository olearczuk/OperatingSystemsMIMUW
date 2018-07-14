Zadanie 6

Biorąc za punkt wyjścia MINIX-owe sterowniki urządzeń umieszczone w katalogu
/usr/src/minix/drivers/examples, zaimplementuj sterownik urządzenia
/dev/hello_stack działający zgodnie z poniższą specyfikacją.

Działanie urządzenia ma przypominać prymitywny stos.

W chwili początkowej urządzenie ma dysponować buforem o pojemności DEVICE_SIZE
bajtów. Stała ta jest zdefiniowana w dostarczanym przez nas pliku nagłówkowym
hello_stack.h.

Pamięć na bufor powinna być rezerwowana (i zwalniana) dynamicznie.

Po uruchomieniu sterownika poleceniem service up ... wszystkie elementy bufora
mają zostać wypełnione wielokrotnością ciągu kodów ASCII liter a, b, i c.
Jeśli wielkość bufora nie jest podzielna przez 3, ostatnie wystąpienie ciągu
powinno zostać odpowiednio skrócone. Wskaźnik stosu powinien w tym momencie
informować o całkowitym zajęciu stosu.

Czytanie z urządzenia za pomocą funkcji read ma powodować odczytanie
z wierzchołka stosu wskazanej liczby bajtów. O ile na stosie nie znajduje się
dostateczna liczba bajtów, należy odpowiednio zredukować parametr polecenia
czytania. Kolejność odczytanych bajtów powinna odpowiadać kolejności, w jakiej
bajty te były zapisywane na stosie. O ile po odczytaniu wskazanego ciągu bajtów
bufor będzie zajęty w co najwyżej jednej czwartej, należy zmniejszyć jego
wielkość o połowę. W przypadku wielkości nieparzystej, nowa wielkość powinna
zostać zaokrąglona w dół do wartości całkowitej.

Operacja pisania do urządzenia za pomocą funkcji write ma powodować zapisanie
wskazanego ciągu bajtów na wierzchołku stosu. O ile wielkość bufora nie
wystarcza do zapisania wskazanego ciągu, należy pojemność bufora podwoić.

Wykonanie funkcji lseek nie powinno powodować zmian w działaniu urządzenia.

Ponadto urządzenie powinno zachowywać aktualny stan w przypadku przeprowadzenia
jego aktualizacji poleceniem service update.

Sterownik będzie kompilowany za pomocą dostarczonego przez nas Makefile,
analogicznego jak dla przykładowego sterownika hello. Plik z rozwiązaniem
hello_stack.c oraz dostarczone przez nas pliki hello_stack.h i Makefile zostaną
umieszczone w katalogu /usr/src/minix/drivers/examples/hello_stack, gdzie będą
wykonywane polecenia:

make clean
make
make install

service up /service/hello_stack
service update /service/hello_stack
service down hello_stack

Testy będą polegały na wykonywaniu operacji open, read, write i close na
urządzeniu /dev/hello_stack. Urządzenie to zostanie utworzone przez skrypt
testujący, a w pliku /etc/system.conf zostanie dla niego utworzony analogiczny
wpis jak dla urządzenia /dev/hello.
