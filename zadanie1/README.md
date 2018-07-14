Zadanie 1

Na potrzeby tego zadania definiujemy zbiór N zawierający liczby całkowite od 0
do 255. Mówimy, że ciąg elementów zbioru N jest poprawny, jeśli istnieje taki
podzbiór M zbioru N, nie zawierający zera, że dany ciąg jest postaci

(permutacja M, 0, permutacja M, 0, ..., permutacja M, 0).

Przykłady poprawnych ciągów:

(0),
(0, 0),
(1, 2, 0, 1, 2, 0),
(1, 2, 0, 2, 1, 0).

Przykłady ciągów, które nie są poprawne:

(),
(1),
(1, 1, 0),
(1, 2, 0, 1, 0),
(1, 2, 0, 1, 3, 0).

Napisz w asemblerze x86_64 program, który jako argument przyjmuje nazwę pliku
i sprawdza, czy ten plik zawiera poprawną binarną reprezentację ciągu. Każdy
bajt pliku interpretujemy jako 8-bitową liczbę bez znaku reprezentującą jeden
element ciągu.

Program niczego nie wypisuje. Program zwraca wartość (ang. exit code) 0, jeśli
wczytany ciąg jest poprawny, a 1 w przeciwnym przypadku. Ponadto program zwraca
1, gdy podano złą liczbę argumentów lub podany argument jest niepoprawny.