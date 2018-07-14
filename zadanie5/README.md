
Zadanie 5

Zadanie polega na zmodyfikowaniu serwera mfs obsługującego system plików MINIX (MINIX File System), tak aby powstał „złośliwy system plików”.
Złośliwy system plików

W poniższym opisie słowo plik jest używane w znaczeniu zwykłego pliku, nie katalogu. Serwer obsługujący „złośliwy system plików” działa tak samo jak normalny mfs za wyjątkiem operacji usuwania. Jeśli użytkownik będzie próbował usunąć plik (nie katalog), to:

    Jeśli w nazwie pliku jest "haha", plik nie zostanie usunięty, ale operacja usuwania zakończy się sukcesem (tzn. zwróci status oznaczający brak błędu). Przykład:

    # touch ./haha
    # ls
    haha
    # rm ./haha; echo $?
    0
    # ls
    haha

    (Polecenie echo $? wyświetla status zakończenia (exit status) poprzedniego polecenia).

    Jeśli w nazwie pliku jest "hehe", działanie zależy od rozmiaru pliku. Jeśli jest on nie większy niż rozmiar bloku (w tym konkretnym systemie plików), to plik jest normalnie usuwany. W przeciwnym przypadku plik jest skracany do wielkości rozmiaru bloku. Skracanie ma nie zmieniać czasu modyfikacji pliku (ctime). Obsługa atime i mtime pozostaje bez zmian. Ewentualne niepowodzenie operacji skracania ma być ignorowane. Na przykład dla systemu plików o rozmiarze bloku równym 4096:

    # dd if=/dev/zero  of=hehe bs=1 count=4097
    4097+0 records in
    4097+0 records out
    # ls
    hehe
    # stat ./hehe 
    785 2 -rw-r--r-- 1 root operator 0 4097 "May 12 17:57:18 2018" "May 12 17:57:18 2018" "May 12 17:57:18 2018" "Jan  1 00:00:00 1970" 4096 16 0 ./hehe
    # rm ./hehe; echo $?
    0
    # ls
    hehe
    # stat ./hehe 
    785 2 -rw-r--r-- 1 root operator 0 4096 "May 12 17:57:18 2018" "May 12 17:57:33 2018" "May 12 17:57:18 2018" "Jan  1 00:00:00 1970" 4096 8 0 ./hehe
    # rm ./hehe; echo $?
    0
    # ls
    #

    Jeśli w nazwie pliku jest "hihi", plik jest usuwany, ale automatycznie jest tworzony nowy, pusty plik w tym samym katalogu o nazwie powstałej przez zmianę pierwszego znaku nazwy usuwanego pliku na znak podkreślenia. Nowo tworzony plik ma typ, uprawnienia itd. (mode) takie, jak usuwany. Jeśli stworzenie nowego pliku jest niemożliwe (np. plik o takiej samej nazwie już istnieje), to całe usuwanie ma się nie powieść i zakończyć z odpowiednim kodem błędu. Przykład:

    # touch ./hihi
    # ls
    hihi
    # rm ./hihi; echo $?
    0
    # ls
    _ihi

    W pozostałych przypadkach usuwanie działa normalnie.

Wymagania i niewymagania

    Poprzez usuwanie pliku rozumiemy operację, jaką wykonuje serwer obsługujący system plików podczas użycia polecenia rm w powłoce lub jego odpowiednika w jakimś języku programowania.
    Sprawdzanie, czy nazwa pliku zawiera „haha”, „hehe” lub „hihi” ma uwzględniać wielkość liter.
    Modyfikacje nie mogą powodować błędów w systemie plików – ma być on zawsze poprawny.
    Jeśli w nazwie pliku można dopasować więcej niż jednej ze specjalnych fragmentów, usuwanie może się zachować w dowolny sposób (ale niepsujący poprawności systemu plików).
    Modyfikacje mogą dotyczyć tylko serwera mfs (czyli mogą dotyczyć tylko plików w katalogu /usr/src/minix/fs/mfs).
    Podczas działania zmodyfikowany serwer nie może wypisywać żadnych dodatkowych informacji (na konsolę ani do rejestru zdarzeń (ang. log).
    Można założyć, że w testowanych przypadkach użytkownik będzie miał wystarczające uprawnienia do wykonania usuwania i wszystkich innych operacji, których potrzebuje wykonać zmodyfikowane usuwanie.
    Można założyć, że w testowanych przypadkach w systemie plików będą tylko zwykłe pliki (nie łącza, nie pseudo-urządzenia itp.) i katalogi.
