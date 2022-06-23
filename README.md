Autor: Piotr Piotrowski

TREŚĆ ZADANIA

Twoim zadaniem jest napisanie prostej aplikacji konsolowej do śledzenia zużycia procesora.
1. Pierwszy wątek (Reader) czyta /proc/stat i wysyła odczytany ciąg znaków (jako raw data) do wątku
drugiego (Analyzer)
2. Wątek drugi (Analyzer) przetwarza dane i wylicza zużycie procesa (wyrażone w %) dla każdego rdzenia
procesora widocznego w /proc/stat i wysyła przetworzone dane do wątku trzeciego (Printer)
3. Wątek trzeci (Printer) drukuje na ekranie w sposób sformatowany (format dowolny, ważne aby był
przejrzysty) średnie zużycie procesora co sekunde
4. Wątek czwarty (Watchdog) pilnuje aby program się nie zawiesił. Tzn jeśli wątki nie wyślą informacji
przez 2 sekundy o tym, że pracują to program kończy działanie z odpowiednim komunikatem błędu
5. Wątek piąty (Logger) przyjmuje wiadomości od wszystkich wątków i zapisuje wiadomości do pliku.
Loggera używa się do zapisywania debug printów do pliku w sposób zsynchronizowany
6. Należy także zaimplementować przechwytywanie sygnału SIGTERM i zadbać o odpowiednie zamknięcie
aplikacji (zamknięcie pliku, zwolnienie pamięci, zakończenie wątków)
