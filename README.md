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
- fmt - formatowanie tekstu
- httplib - serwer/klient HTTP

Dane pogodowe pobierane są z API open-meteo.com.

### 2. Dockerfile

Plik Dockerfile został opracowany z uwzględnieniem najlepszych praktyk:

#### Techniki optymalizacji wykorzystane w Dockerfile:
- **Budowanie wieloetapowe**: Wykorzystano dwa etapy - pierwszy do budowy aplikacji, drugi do stworzenia minimalnego obrazu końcowego
- **Warstwy scratch**: Obraz końcowy bazuje na warstwie scratch dla minimalnego rozmiaru
- **Optymalizacja cache-a**: Najpierw kopiowane są pliki niezbędne do budowy, następnie instalowane zależności
- **Flagi kompilacji**: Zastosowano flagi kompilacji optymalizujące rozmiar pliku wykonywalnego:
  - `-Oz` - optymalizacja pod kątem rozmiaru
  - `-ffunction-sections`, `-fdata-sections` - umożliwia usunięcie nieużywanego kodu
  - `-fvisibility=hidden` - ograniczenie eksportowanych symboli
  - `-flto` - optymalizacja na etapie linkowania
  - `-fno-exceptions`, `-fno-rtti` - wyłączenie obsługi wyjątków i RTTI dla mniejszego rozmiaru
- **Optymalizacja linkera**: Użyto statycznej kompilacji i usuwania nieużywanych sekcji
- **Strip**: Usunięcie symboli debugowania z pliku wynikowego

### 3. Polecenia

#### a. Zbudowanie obrazu kontenera
```bash
docker build -t zad1-v1 .
```

#### b. Uruchomienie kontenera
```bash
docker run -d -p 3000:3000 --name zad1-v1 zad1-v1
```

#### c. Uzyskanie informacji z logów
```bash
docker logs zad1-v1
```

#### d. Sprawdzenie liczby warstw i rozmiaru obrazu
```bash
# Sprawdzenie rozmiaru obrazu
docker image ls zad1-v1

# Sprawdzenie liczby warstw
docker history zad1-v1
```

### Przykładowe wyniki

#### Logi aplikacji (po uruchomieniu)
```
2025-05-06 21:04:25.492744900
Konrad Nowak
port: 3000
```

#### Informacja o obrazie
```
REPOSITORY   TAG       IMAGE ID       CREATED          SIZE
zad1-v1      latest    35d107a2e5cf   23 minutes ago   1.29MB
```
Ostateczna wielkość obrazu to **`1.29MB`**. Sam plik binarny zajmuje **`893kB`** - z pewnością da się zejść do mniejszych wartości, jednak żeby tak się stało to trzebaby się bawić, a na to trzeba mieć czas. Jedank wydaje mi się, że taka wielkość zachowując interfejs graficzny w przeglądarce jest wystarczająca.

#### Warstwy obrazu
```
IMAGE          CREATED          CREATED BY                                      SIZE      COMMENT
35d107a2e5cf   23 minutes ago   ENTRYPOINT ["/app/weather"]                     0B        buildkit.dockerfile.v0
<missing>      23 minutes ago   EXPOSE map[3000/tcp:{}]                         0B        buildkit.dockerfile.v0
<missing>      23 minutes ago   COPY /src/build/bin/main /app/weather # buil…   893kB     buildkit.dockerfile.v0 
```

#### Użycie aplikacji
Po uruchomieniu kontenera aplikacja jest dostępna pod adresem `http://localhost:3000`. Interfejs umożliwia wybór kraju i miasta, a następnie wyświetla aktualną temperaturę i prędkość wiatru dla wybranej lokalizacji.
