# Ogolne informacje

Zadania wykonane w zupełności. Założyłem poprawność wpisywanych komend w zadaniu 2 (create_table itd.) więc nie przeprowadzałem żadnej ich walidacji.

W samej bibliotece jak i plikach testowych są dostępne dodatkowe funkcje takie jak 
```
    void printPairArray(PairArray *pairArray)
    void printBlockArray(BlockArray *blockArray)
    void printFile(FILE* file)
```
które znacznie ułatwiają debugowanie, lub też ewentualne sprawdzanie zawartości naszych struktur czy też plików w poszczególnych miejscach naszego programu.

Punkt ```"Na przemian  kilkakrotne dodanie i usunięcie zadanej liczby bloków"``` został zrealizowany 1000 razy w każdym cyklu. Zmiana zmiennej

    REMOVE_AND_ADD_BLOCKS    

w plikach testowych (zad2.c zad3.c).

# Makefile komendy

* raport2 - testy dla zadania 2, wszystko zapisywane jest do pliku raport2.txt
* raport3 - testy dla zadania 3, wszystko zapisywane jest do pliku raport3.txt
* main2Static - kompilacja zadania 2 wraz z dołączoną statycznie biblioteką
* main2Shared - kompilacja zadania 2 wraz z dołączoną dzieloną biblioteką
* main3Dynamic - kompolacja zadania 3 wraz z dołączoną dynamicznie bibliteką
* run2 / 3 - uruchomienie odpowiedniego zadania (używaj tylko poprzedzając odpowienią kompilacja!)
* main1 - kompilacja oraz uruchomienie zadania 1
