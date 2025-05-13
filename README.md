# Zadanie 1

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

## CZĘŚĆ NIEOBOWIĄZKOWA

### Budowa obrazu na 2 architektury
Przygotowałem plik `Dockerfile_multi`, który już bez większego przywiązania do wielkości wynikowego obrazu, pozwala na jego zbudowanie. Korzystamy z pluginu `buildx`, aby stworzyć obraz na dwie platformy.

#### Tworzenie `buildx`
```bash
docker buildx create --use --name mybuilder
```

#### Sprawdzenie sterownika
```bash
docker buildx inspect --bootstrap
```
Wynik:
```bash
Name:          mybuilder
Driver:        docker-container
Last Activity: 2025-05-13 19:58:08 +0000 UTC
# ...
```

#### Zbudowanie obrazu i opublikowanie go na repozytorium
Stworzyłem publiczne [repozytorium](https://hub.docker.com/repository/docker/konradnowakpollub/weather). Do którego automatycznie został dodany obraz po zbudowaniu.
Polecenie użyte do budowania (precyzujemy architektury):
```bash
docker buildx build --platform linux/amd64,linux/arm64 -t konradnowakpollub/weather:latest --push .
```

#### Sprawdznie manifestu wynikowego obrazu
Można to zrobić używając polecenia:
```bash
docker buildx imagetools inspect konradnowakpollub/weather:latest
```
Uzyskany wynik w konsoli:
```bash
zadanie1> docker buildx imagetools inspect konradnowakpollub/weather:latest
Name:      docker.io/konradnowakpollub/weather:latest
MediaType: application/vnd.oci.image.index.v1+json
Digest:    sha256:3c8eff38372fa468457e3c113d0006c78392020b4cf9e93c913d214013c6e43b

Manifests:
  Name:        docker.io/konradnowakpollub/weather:latest@sha256:17183929d873f42809d70f9cf886f7c07d6cf75b536dadba1857d95b08c26b42
  MediaType:   application/vnd.oci.image.manifest.v1+json
  Platform:    linux/amd64

  Name:        docker.io/konradnowakpollub/weather:latest@sha256:e35d7c0a3b0f2bd80cda34e6c020e27b9060f438b1bb35d16f27a6919f0ebed3
  MediaType:   application/vnd.oci.image.manifest.v1+json
  Platform:    linux/arm64
# ...
```
Jak można zobaczyć w wyżej wymienionym fragmencie kodu, manifest zawiera deklaracje dla dwóch platform sprzętowych.