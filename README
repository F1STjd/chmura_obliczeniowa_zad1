# Zadanie 1 - Weather Application

## Autor: Konrad Nowak

## CZĘŚĆ OBOWIĄZKOWA

### 1. Aplikacja pogodowa

Opracowana aplikacja napisana jest w C++ i zapewnia następującą funkcjonalność:
- Po uruchomieniu wyświetla w logach:
  - Datę i czas uruchomienia
  - Imię i nazwisko autora (Konrad Nowak)
  - Port TCP, na którym nasłuchuje aplikacja (3000)
- Interfejs webowy umożliwiający:
  - Wybór kraju z predefiniowanej listy
  - Wybór miasta w wybranym kraju
  - Wyświetlenie aktualnej pogody (temperatury i prędkości wiatru) dla wybranej lokalizacji

Aplikacja korzysta z następujących bibliotek:
- expresscpp - obsługa serwera HTTP
- fmt - formatowanie tekstu
- httplib - klient HTTP do pobierania danych pogodowych
- nlohmann/json - obsługa danych JSON

Dane pogodowe pobierane są z API open-meteo.com.

### 2. Dockerfile

Plik Dockerfile został opracowany z uwzględnieniem najlepszych praktyk:

#### Techniki optymalizacji wykorzystane w Dockerfile:
- **Budowanie wieloetapowe**: Wykorzystano dwa etapy - pierwszy do budowy aplikacji, drugi do stworzenia minimalnego obrazu końcowego
- **Warstwy scratch**: Obraz końcowy bazuje na warstwie scratch dla minimalnego rozmiaru
- **Optymalizacja cache-a**: Najpierw kopiowane są pliki niezbędne do budowy, następnie instalowane zależności
- **Flagi kompilacji**: Zastosowano flagi kompilacji optymalizujące rozmiar pliku wykonywalnego:
  - `-Os` - optymalizacja pod kątem rozmiaru
  - `-ffunction-sections`, `-fdata-sections` - umożliwia usunięcie nieużywanego kodu
  - `-fvisibility=hidden` - ograniczenie eksportowanych symboli
  - `-flto` - optymalizacja na etapie linkowania
  - `-fno-exceptions`, `-fno-rtti` - wyłączenie obsługi wyjątków i RTTI dla mniejszego rozmiaru
- **Optymalizacja linkera**: Użyto statycznej kompilacji i usuwania nieużywanych sekcji
- **Strip**: Usunięcie symboli debugowania z pliku wynikowego

### 3. Polecenia

#### a. Zbudowanie obrazu kontenera
```bash
docker build -t weather-app:latest .