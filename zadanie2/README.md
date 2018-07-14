Zadanie 2

Zaimplementuj w asemblerze x86_64 uogólniony semafor, który może być używany
w programach napisanych w języku C. Implementacja semafora ma się składać
z trzech funkcji, które w C będą widziane jako:

void proberen(int32_t *semaphore, int32_t value);
void verhogen(int32_t *semaphore, int32_t value);
uint64_t proberen_time(int32_t *semaphore, int32_t value);

Parametr semaphore jest adresem zmiennej przechowującej wartość semafora.
Parametr value jest liczbą całkowitą z przedziału od 1 do 2^31 - 1.

Funkcja proberen sprawdza, czy wartość semafora jest większa lub równa value.
Jeśli nie jest, to aktywnie czeka. Czekanie to nie powinno blokować dostępów
do pamięci, czyli nie powinno wykorzystywać instrukcji z prefiksem lock ani też
instrukcji xchg. Jeśli wartość semafora jest większa lub równa value, to funkcja
próbuje ją atomowo zmniejszyć. Można do tego wykorzystać instrukcję lock xadd.
Jeśli zmniejszenie się powiodło, następuje wyjście z funkcji. Zmniejszenie może
się nie powieść, bo jakiś inny wątek, też czekający na tym samym semaforze, może
być szybszy i zmniejszyć wartość semafora poniżej value. W takim przypadku
funkcja wraca do aktywnego oczekiwania na zwiększenie wartości semafora.

Funkcja verhogen zwiększa atomowo wartość semafora o value. Można do tego
wykorzystać instrukcję lock add.

Funkcja proberen_time woła funkcję proberen z tymi samymi argumentami, które
dostała, i mierzy (zwraca) czas przebywania sterowania w proberen. W tym celu
wała dwukrotnie funkcję

uint64_t get_os_time(void);

Pierwszy raz woła ją na początku przed wywołaniem proberen, a drugi – tuż przed
zakończeniem, gdy nastąpi powrót z proberen. Funkcja get_os_time daje liczbę
tyknięć zegara systemowego. Funkcja proberen_time oblicza czas działania jako
różnicę tych wartości.

Z powyższego opisu wynika, że wątek czekający na semaforze na mniejszą wartość
może zagłodzić wątek czekający na większą wartość i godzimy się z tym.