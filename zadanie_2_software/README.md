# Symulator magistrali CAN 2.0A

Prosty symulator magistrali **CAN 2.0A (500 kbit/s)** napisany w C++17. Projekt symuluje komunikację czterech modułów systemu pokładowego z komputerem głównym.

## Funkcjonalności

- Okresowe generowanie wiadomości CAN.
- Arbitraż na podstawie identyfikatora (niższe ID = wyższy priorytet).
- Kolejkowanie wiadomości i symulacja zajętości magistrali.
- Obliczanie czasu transmisji z uwzględnieniem narzutu protokołu i oszacowania bit stuffingu.
- Statystyki opóźnień i przekroczonych terminów.
- Symulacja zdarzeniowa bez używania rzeczywistych opóźnień.

## Konfiguracja

| Moduł | Wiadomość | CAN ID | Dane | Okres | Deadline |
|---|---|---|---|---|---|
| Silnik | Temperatura | `0x010` | 4 B | 10 ms | 50 ms |
| Silnik | Ciśnienie | `0x020` | 4 B | 20 ms | 10 ms |
| Zasilanie | Napięcie | `0x100` | 2 B | 100 ms | — |
| Nawigacja | GPS | `0x200` | 8 B | 200 ms | 100 ms |
| Diagnostyka | Status | `0x300` | 4 B | 500 ms | — |

## Kompilacja i uruchomienie

Wymagane: kompilator C++17 oraz CMake 3.16+.

```bash
cmake -S . -B build
cmake --build build
./build/can_sim
```

## Wyniki

Dla 1 sekundy generowania wiadomości:

| CAN ID | Wiadomości | Średnie opóźnienie | Maksymalne opóźnienie | Przekroczenia |
|---|---:|---:|---:|---:|
| `0x010` | 100 | 190 µs | 190 µs | 0 |
| `0x020` | 50 | 380 µs | 380 µs | 0 |
| `0x100` | 10 | 530 µs | 530 µs | 0 |
| `0x200` | 5 | 800 µs | 800 µs | 0 |
| `0x300` | 2 | 855 µs | 990 µs | 0 |

Wszystkie wiadomości zostały dostarczone w wymaganym czasie.

## Założenia

Symulator nie implementuje fizycznej komunikacji CAN. Czas transmisji obliczany jest na podstawie formatu ramki CAN 2.0A, z konserwatywnym oszacowaniem narzutu bit stuffingu zamiast dokładnej analizy zawartości bitowej.

Projekt wykorzystuje wyłącznie standardową bibliotekę C++.