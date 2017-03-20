## Producet-Consumer ##
**Producent-konsument, problematyka:**

Jedną grupę stanowią **producenci**, których zadaniem jest umieszczanie produktów we współdzielonym obszarze pamięci. Drugą grupą są **konsumenci**, którzy produkty te pobierają i usuwają z bufora - lub też pobierają towar (nie niszcząc obszaru pamięci) i konsumują go. Aby panowała harmonia, a operacje nie interferowały ze sobą,  akcje te muszą być zsynchronizowane.
Muszą zatem być spełnione warunki:

1. Każda wyprodukowana jednostka towaru musi zostać skonsumowana.
2. Żadna jednostka towaru nie może być skonsumowana więcej niż raz.

**Projekt**

Rozwiązanie problemu opiera się o mechanizm semaforów. Dla zademonstrowania, że nie doszło do utraty lub zwielokrotnienia towaru, producent wczytuje dane z pliku tekstowego, a konsument umieszcza pobrany towar w innym pliku z danymi.
Do symulacji prędkości działania programów użyta została funkcja sleep().