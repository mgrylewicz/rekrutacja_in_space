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

### Obsługiwane typy danych

| Typ | Przykład |
|---|---|
| `int` | `100` |
| `float` | `24.5` |
| `bool` | `true` / `false` |
| `string` | `Rocket` |

## Dostępne komendy CLI

| Komenda | Opis |
|---|---|
| `save` | Zapis nowej zmiennej |
| `change` | Zmiana wartości istniejącej zmiennej |
| `read` | Odczyt zmiennej |
| `delete` | Usunięcie zmiennej |
| `show` | Wyświetlenie wszystkich zmiennych |
| `help` | Wyświetlenie dostępnych komend |

### Przykład użycia

```text
Command: save
Insert your variable:
*type* *name* *value*
int counter 100
Variable saved!

Command: read
counter
type: int
name: counter
value: 100

Command: change
counter 200
Variable updated!
Old value: 100
New value: 200

Command: show
Saved variables:
int counter 200
```

Jeżeli użytkownik spróbuje ustawić tę samą wartość, program pominie zapis:

```text
Command: change
counter 200
Value unchanged. NVS write skipped!
```

## Wartości domyślne

Przy uruchomieniu program sprawdza, czy istnieją następujące zmienne:

| Nazwa | Typ | Wartość domyślna |
|---|---|---|
| `engine_power` | `int` | `50` |
| `fuel_limit` | `float` | `75.5` |
| `safety_mode` | `bool` | `true` |
| `project_name` | `string` | `Flash` |

Jeżeli zmienna nie istnieje, program zapisuje jej wartość domyślną. Istniejące wartości nie są nadpisywane podczas restartu.

## Ograniczenia pamięci Flash

Pamięć Flash ma ograniczoną liczbę cykli zapisu i kasowania.

W projekcie wykorzystano NVS, które zarządza zapisem danych oraz odzyskiwaniem miejsca w pamięci Flash. Dzięki temu program nie musi samodzielnie zarządzać sektorami pamięci.

Dodatkowo podczas modyfikowania zmiennych program porównuje nową wartość z poprzednią. Jeżeli wartości są identyczne, operacja zapisu do NVS jest pomijana.

Pozwala to ograniczyć niepotrzebne operacje zapisu.

## Struktura projektu

```text
pamiec_flash/
├── CMakeLists.txt
├── main/
│   ├── main.c
│   └── CMakeLists.txt
└── components/
    ├── flash_manage/
    │   ├── flash_manage.c
    │   ├── flash_manage.h
    │   └── CMakeLists.txt
    └── CLI/
        ├── interface_actions.c
        ├── interface_actions.h
        ├── interface_view.c
        ├── interface_view.h
        └── CMakeLists.txt
```

- **flash_manage** – obsługa pamięci NVS i operacji na zmiennych.
- **CLI** – obsługa komend użytkownika, menu i komunikacji z terminalem.
- **main** – inicjalizacja pamięci, UART i uruchomienie programu.

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

## Autor

[Maciej Grylewicz]