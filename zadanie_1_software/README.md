# ESP32 Flash Memory Manager

## Opis projektu

Program umożliwia zapisywanie, odczytywanie, modyfikowanie i usuwanie zmiennych konfiguracyjnych w pamięci nieulotnej Flash mikrokontrolera ESP32.

Do przechowywania danych wykorzystano bibliotekę **NVS (Non-Volatile Storage)** dostępną w ESP-IDF. Dzięki temu zapisane wartości pozostają dostępne również po ponownym uruchomieniu mikrokontrolera.

Obsługa programu odbywa się przez prosty interfejs CLI w terminalu UART.

## Funkcjonalności

- Zapisywanie nowych zmiennych.
- Odczytywanie zapisanych wartości.
- Modyfikowanie istniejących zmiennych.
- Usuwanie zmiennych.
- Wyświetlanie wszystkich zapisanych zmiennych wraz z typem i wartością.
- Obsługa błędnych danych i nieistniejących zmiennych.
- Automatyczne ustawianie wartości domyślnych, jeżeli zmienne nie istnieją.
- Pomijanie zapisu do NVS, gdy nowa wartość jest identyczna z poprzednią.


## Ograniczenia pamięci Flash

Pamięć Flash ma ograniczoną liczbę cykli zapisu i kasowania.

W projekcie wykorzystano NVS, które zarządza zapisem danych oraz odzyskiwaniem miejsca w pamięci Flash. Dzięki temu program nie musi samodzielnie zarządzać sektorami pamięci.

Dodatkowo podczas modyfikowania zmiennych program porównuje nową wartość z poprzednią. Jeżeli wartości są identyczne, operacja zapisu do NVS jest pomijana.

Pozwala to ograniczyć niepotrzebne operacje zapisu.


## Kompilacja i uruchomienie

Wymagane jest środowisko ESP-IDF oraz mikrokontroler ESP32.

1. Otwórz terminal ESP-IDF.
2. Przejdź do katalogu projektu.
3. Ustaw odpowiedni model mikrokontrolera (przykładowo ESP32).
4. Zbuduj i wgraj program.

```bash
idf.py set-target esp32
idf.py build
idf.py -p PORT flash monitor
```

Zastąp `PORT` nazwą portu szeregowego, do którego podłączony jest mikrokontroler.

Po uruchomieniu programu w terminalu pojawi się menu. Komendy można wpisywać po komunikacie `Command:`.

## Testowanie na sprzęcie

Projekt został przetestowany na **fizycznym mikrokontrolerze ESP32**, a nie tylko w środowisku symulacyjnym.

Program został pomyślnie skompilowany i uruchomiony na płytce ESP32 z wykorzystaniem ESP-IDF. Komunikacja z użytkownikiem odbywała się przez terminal UART.

Podczas testów sprawdzono działanie interfejsu CLI oraz operacji na zmiennych zapisanych w pamięci NVS:
- Zapisywanie zmiennych różnych typów.
- Odczytywanie i modyfikowanie zapisanych wartości.
- Usuwanie zmiennych.
- Wyświetlanie wszystkich zapisanych zmiennych.
- Obsługę niepoprawnych danych.
- Pomijanie zapisu do NVS, gdy wartość zmiennej nie uległa zmianie.

Program wykorzystuje rzeczywistą pamięć Flash mikrokontrolera ESP32.
